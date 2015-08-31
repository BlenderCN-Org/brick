#ifndef __LIFE_FORM_H__
#define __LIFE_FORM_H__

#include "object/object.h"

class Lifeform2D : public Object2D
{
    public:
        Lifeform2D();
        ~Lifeform2();

        virtual void render_gl();
        virtual void init();
        virtual void simulate(const float t);

#ifdef __EDITOR__
	virtual bool hit_test_2d(float p[2]) { return false; }
#endif //__EDITOR__

    protected:
        //physical health
        float overall_health;       //basically, hit points

        float glucose_level;        //quick burn energy
        float protein_level;        //slow burn energy
        float hydration_level;      //hydration
        float metabolism_speed;     //how fast we burn through fuel

        float temperature;          //

        float age;
};

#endif // __LIFE_FORM_H__
