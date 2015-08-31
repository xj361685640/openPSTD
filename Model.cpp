//
// Created by michiel on 18-7-2015.
//

#include "Model.h"

bool InvalidationDataItemI::IsChanged()
{
    return changed;
}
void InvalidationDataItemI::Change()
{
    changed = true;
}
void InvalidationDataItemI::Reset()
{
    changed = false;
}

void InvalidationData::Register(std::weak_ptr<InvalidationDataItemI> item)
{
    this->items.push_back(item);
}

bool InvalidationData::IsChanged()
{
    auto i = std::begin(items);

    while (i != std::end(items))
    {
        if (auto observe = i->lock())
        {
            if(observe->changed)
                return true;
            i++;
        }
        else
        {
            items.erase(i);
        }

    }
    return false;
}

void InvalidationData::Reset()
{
    auto i = std::begin(items);

    while (i != std::end(items))
    {
        if (auto observe = i->lock())
        {
            observe->Reset();
            i++;
        }
        else
        {
            items.erase(i);
        }
    }
}

Model::Model() : interactive(new InteractiveModel()), view(new InvalidationDataItem<QMatrix4x4>()),
                 colorScheme(new InvalidationDataItem<std::unique_ptr<BaseColorScheme>>()),
                 snapping(new SnappingSettings())
{
    this->invalidation.Register(interactive);
    this->invalidation.Register(view);
    this->invalidation.Register(colorScheme);
    this->invalidation.Register(snapping);
}

