/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include <vector>
#include <string>

namespace solo
{
    class VertexAttribute final
    {
    public:
        std::string name;
        uint32_t elementCount;
        uint32_t size;
        uint32_t location;
        uint32_t offset;
    };

    class VertexBufferLayout final
    {
    public:
        // TODO "addPrefabAttribute" with "semantics", which simply translates "position" into 0, "normal" into 1, etc.
        void addAttribute(uint32_t elementCount, uint32_t location);
        void addNamedAttribute(uint32_t elementCount, const std::string &name);

        auto getAttributeCount() const -> uint32_t { return static_cast<uint32_t>(attrs.size()); }
        auto getAttribute(uint32_t index) const -> VertexAttribute { return attrs.at(index); }

        auto getSize() const -> uint32_t { return size; }

    private:
        std::vector<VertexAttribute> attrs;
        uint32_t size = 0;
    };

    inline void VertexBufferLayout::addAttribute(uint32_t elementCount, uint32_t location)
    {
        const auto size = static_cast<uint32_t>(sizeof(float) * elementCount);
        const auto offset = attrs.empty() ? 0 : attrs.crbegin()->offset + attrs.crbegin()->size;
        attrs.push_back(VertexAttribute{"", elementCount, size, location, offset});
        this->size += size;
    }

    inline void VertexBufferLayout::addNamedAttribute(uint32_t elementCount, const std::string &name)
    {
        const auto size = static_cast<uint32_t>(sizeof(float) * elementCount);
        const auto offset = attrs.empty() ? 0 : attrs.crbegin()->offset + attrs.crbegin()->size;
        attrs.push_back(VertexAttribute{name, elementCount, size, 0, offset});
        this->size += size;
    }
}
