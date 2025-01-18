#pragma once

#include <grapher/grapher.h>

class FileOutputSlot : public BaseSlot {
    typedef BaseSlot Parent;

private:
public:
    explicit FileOutputSlot(const BaseStyle& vParentStyle);
    bool init() override;
};
