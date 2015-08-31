#ifndef __PLANT_H__
#define __PLANT_H__

#include "object/object.h"

enum PlantLifeCycleState
{
    PLANT_LIFECYCLE_SEED,       //dormant, waiting to sprout
    PLANT_LIFECYCLE_SHRUB,      //growing, but unable to produce fruit
    PLANT_LIFECYCLE_MATURE,     //fruiting, no longer growing
    PLANT_LIFECYCLE_DYING,      //unable to frut, dying

    NUM_PLANT_LIFECYCLES
};

class World;

class Plant : public Object2D
{
    public:
        Plant();
        ~Plant();

        virtual void render_gl();

        virtual void init();
        virtual void simulate(const float t);

        void set_world(World *w);

    protected:
        Material *tmp_mat;
        float age;
        float life_cycle_ages[NUM_PLANT_LIFECYCLES];

        PlantLifeCycleState life_cycle_state;

        float fruiting_rate;            //how often this plant will drop a seed
        float time_since_last_fruit;

        World *world;   //kinda hacky... :(
};


#endif // __PLANT_H__
