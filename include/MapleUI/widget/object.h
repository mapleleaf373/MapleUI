#pragma once
#include "define.h"

namespace maple
{

    class Object
    {
    protected:
        Object(std::shared_ptr<Object> parent_);
        virtual ~Object();
    protected:
        consteval virtual bool parentable() = 0;

    public:
        virtual void render() = 0;

        
    };

    class Frame : public Object
    {
    protected:
        Frame(std::shared_ptr<Object> parent_);
        virtual ~Frame() override;
    protected:
        consteval virtual bool parentable() { return true; };

    public:
        virtual void render() override;
    };

}