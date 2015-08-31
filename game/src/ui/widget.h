#ifndef _WIDGET_H_
#define _WIDGET_H_

class Widget
{
    public:
        Widget();
        ~Widget();

        virtual void init() = 0;
        virtual void simulate(const float t) = 0;
        virtual void render_gl() = 0;

    protected:
        float x, y, width, height;
};

#endif // _WIDGET_H_
