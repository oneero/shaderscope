#include <bgfx_compute.sh>
#include "uniforms.sh"

BUFFER_RO(prevPositionBuffer,    vec4, 0);
BUFFER_RO(currPositionBuffer,    vec4, 1);
BUFFER_WR(outPrevPositionBuffer, vec4, 2);
BUFFER_WR(outCurrPositionBuffer, vec4, 3);

BUFFER_RO(prevVelocityBuffer,    vec4, 4);
BUFFER_RO(currVelocityBuffer,    vec4, 5);
BUFFER_WR(outPrevVelocityBuffer, vec4, 6);
BUFFER_WR(outCurrVelocityBuffer, vec4, 7);

#define GROUP_SIZE 512
SHARED vec3 otherBoidPosition[GROUP_SIZE];
SHARED vec3 otherBoidVelocity[GROUP_SIZE];

vec3 Random(vec3 p)
{
    vec3 a = frac(p.xyx * vec3(123.34, 234.34, 345.65));
    a += dot(a, a + 34.45);
    return frac(vec3(a.x * a.y, a.y * a.z, a.z * a.x));
}

vec3 RandomDirection(vec3 p)
{
    return (normalize(2.0 * (Random(p) - 0.5)));
}

vec3 CalculateSeparation(vec3 a, vec3 b)
{
    vec3 desired = vec3(.0, .0, .0);

    float d = distance(a, b);
    if (d > 0 && d < u_desiredSeparation)
    {
        vec3 delta = a - b;
        //desired = normalize(delta) / d;
        desired = (delta/d)/d;
    }

    return desired;    
}

NUM_THREADS(GROUP_SIZE, 1, 1)
void main()
{
    vec3 prevPosition = prevPositionBuffer[gl_GlobalInvocationID.x].xyz;
	vec3 currPosition = currPositionBuffer[gl_GlobalInvocationID.x].xyz;
    vec3 prevVelocity = prevVelocityBuffer[gl_GlobalInvocationID.x].xyz;
	vec3 currVelocity = currVelocityBuffer[gl_GlobalInvocationID.x].xyz;

    vec3 acceleration = vec3_splat(0.0);

    uint count = 0;
    int sep_count = 0;

    vec3 sep_sum = vec3_splat(0.0);
    vec3 sep_force = vec3_splat(0.0);
    vec3 ali_sum = vec3_splat(0.0);
    vec3 ali_force = vec3_splat(0.0);
    vec3 coh_sum = vec3_splat(0.0);
    vec3 coh_force = vec3_splat(0.0);


    for (int j = 0; j < int(u_dispatchSize); ++j)
	{
		otherBoidPosition[gl_LocalInvocationIndex] = currPositionBuffer[j * GROUP_SIZE + int(gl_LocalInvocationIndex)].xyz;
        otherBoidVelocity[gl_LocalInvocationIndex] = currVelocityBuffer[j * GROUP_SIZE + int(gl_LocalInvocationIndex)].xyz;

		barrier();
		for (int i = 0; i < GROUP_SIZE; ++i)
		{
            // distance to other boid
            float d = distance(currPosition, otherBoidPosition[i]);

            if (d > 0 && d < u_sensorDistance && count < u_maxNeighbors)
            {
                count = count + 1;

                // Separation sum
                vec3 sep = CalculateSeparation(currPosition, otherBoidPosition[i]);
                sep_sum = sep_sum + sep;
                if (length(sep) > 0) sep_count = sep_count + 1;
                
                // Aligment sum
                ali_sum = ali_sum + otherBoidVelocity[i];

                // Cohesion sum    
                coh_sum = coh_sum + otherBoidPosition[i];
            }
		}
	}

    // Separation force
    if (sep_count > 0)
    {
        sep_sum = sep_sum / (float)sep_count;
        sep_force = sep_sum;
        if (length(sep_force) > u_maxForce) sep_force = normalize(sep_force) * u_maxForce;
    }           
    
    if (count > 0)
    {
        // Aligment force

        ali_sum = ali_sum / (float)count;
        ali_force = ali_sum;
        if (length(ali_force) > u_maxForce) ali_force = normalize(ali_force) * u_maxForce;        

        // Cohesion force

        coh_sum = coh_sum / (float)count;
        
        vec3 coh_dir = coh_sum - currPosition;
        coh_dir = normalize(coh_dir);
        coh_force = coh_dir;
        if (length(coh_force) > u_maxForce) coh_force = normalize(coh_force) * u_maxForce;
    }

    // Towards middle
    vec3 center = vec3(0., 0., 0.); 
    vec3 toCenter = center - currPosition;
    float dToCenter = distance(currPosition, center);

    vec3 pushToCenter = vec3(.0, .0, .0);
    pushToCenter = normalize(toCenter) * dToCenter;
    if (length(pushToCenter) > u_maxForce) pushToCenter = normalize(pushToCenter) * u_maxForce;

    // Wandering
    vec3 targetPos = currPosition + currVelocity;
    vec3 rndPos = targetPos + RandomDirection(currPosition) * u_wanderDistance;
    vec3 w = rndPos - currPosition;
    vec3 wf = w;
    if (length(wf) > u_maxForce) wf = normalize(wf) * u_maxForce;

    // Apply forces
    acceleration = acceleration + (sep_force * u_sepMult);
    acceleration = acceleration + (ali_force * u_aliMult);
    acceleration = acceleration + (coh_force * u_cohMult);
    acceleration = acceleration + (pushToCenter * u_edgeAvoidanceMult);
    acceleration = acceleration + (wf * u_wanderMult);

    // Update velocity and position
    vec3 newVelocity = currVelocity + acceleration * u_timeStep;;
    if (length(newVelocity) > u_maxVelocity) newVelocity = normalize(newVelocity) * u_maxVelocity;

    vec3 newPosition = currPosition + (newVelocity * u_timeStep);

    // Update buffers
	outPrevPositionBuffer[gl_GlobalInvocationID.x] = vec4(currPosition, 0.0);
	outCurrPositionBuffer[gl_GlobalInvocationID.x] = vec4(newPosition, 0.0);
    outPrevVelocityBuffer[gl_GlobalInvocationID.x] = vec4(currVelocity, 0.0);
	outCurrVelocityBuffer[gl_GlobalInvocationID.x] = vec4(newVelocity, 0.0);
}
