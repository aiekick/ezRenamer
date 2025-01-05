#pragma once

#include <graph/base/baseSlot.h>

class FileOutputSlot : public BaseSlot {
    typedef BaseSlot Parent;

private:
public:
    explicit FileOutputSlot(const BaseStyle& vParentStyle);
    bool init() override;
};
