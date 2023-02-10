uniform vec4 u_params[5];

#define threadGroupUpdateSize       512

#define u_timeStep                  u_params[0].x
#define u_dispatchSize              floatBitsToUint(u_params[0].y)
#define u_particleIntensity         u_params[0].z
#define u_particleSize              u_params[0].w

#define u_particlePower             u_params[1].x
#define u_baseSeed                  floatBitsToUint(u_params[1].y)
#define u_initialSpeed              u_params[1].z
#define u_maxNeighbors              floatBitsToUint(u_params[1].w)

#define u_maxVelocity               u_params[2].x
#define u_maxForce                  u_params[2].y
#define u_sensorDistance            u_params[2].x
#define u_desiredSeparation         u_params[2].x

#define u_edgeAvoidanceThreshold    u_params[3].x
#define u_edgeAvoidanceMax          u_params[3].y
#define u_wanderDistance            u_params[3].z
#define u_sepMult                   u_params[3].w

#define u_aliMult                   u_params[4].x
#define u_cohMult                   u_params[4].y
#define u_edgeAvoidanceMult         u_params[4].z
#define u_wanderMult                u_params[4].w