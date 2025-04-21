#include <bx/debug.h>
#include <bx/hash.h>
#include <bx/readerwriter.h>
#include <bx/sort.h>
#include <bx/string.h>
#include <bx/uint32_t.h>

#include "vertexlayout.h"

namespace TinyRender
{

static const uint8_t s_attribTypeSizeD3D1x[AttribType::Count][4] = {
    {1, 2, 4, 4},   // Uint8
    {4, 4, 4, 4},   // Uint10
    {2, 4, 8, 8},   // Int16
    {2, 4, 8, 8},   // Half
    {4, 8, 12, 16}, // Float
};

static const uint8_t s_attribTypeSizeGl[AttribType::Count][4] = {
    {1, 2, 4, 4},   // Uint8
    {4, 4, 4, 4},   // Uint10
    {2, 4, 6, 8},   // Int16
    {2, 4, 6, 8},   // Half
    {4, 8, 12, 16}, // Float
};

static const uint8_t (*s_attribTypeSize[])[AttribType::Count][4] = {
    &s_attribTypeSizeD3D1x, // Noop
    &s_attribTypeSizeD3D1x, // Agc
    &s_attribTypeSizeD3D1x, // Direct3D11
    &s_attribTypeSizeD3D1x, // Direct3D12
    &s_attribTypeSizeD3D1x, // Gnm
    &s_attribTypeSizeGl,    // Metal
    &s_attribTypeSizeGl,    // Nvn
    &s_attribTypeSizeGl,    // OpenGLES
    &s_attribTypeSizeGl,    // OpenGL
    &s_attribTypeSizeD3D1x, // Vulkan
    &s_attribTypeSizeD3D1x, // Count
};
static_assert(BX_COUNTOF(s_attribTypeSize) == RendererType::Count + 1);

void initAttribTypeSizeTable(RendererType::Enum _type)
{
    s_attribTypeSize[0] = s_attribTypeSize[_type];
    s_attribTypeSize[RendererType::Count] = s_attribTypeSize[_type];
}

VertexLayout::VertexLayout()
{
    m_stride = 0;
    m_hash = 0;
}

VertexLayout &VertexLayout::begin(RendererType::Enum _renderer)
{
    m_hash = _renderer; // use hash to store renderer type while building VertexLayout.
    m_stride = 0;
    bx::memSet(m_attributes, 0xff, sizeof(m_attributes));
    bx::memSet(m_offset, 0, sizeof(m_offset));

    return *this;
}

void VertexLayout::end()
{
    bx::HashMurmur2A murmur;
    murmur.begin();
    murmur.add(m_attributes, sizeof(m_attributes));
    murmur.add(m_offset, sizeof(m_offset));
    murmur.add(m_stride);
    m_hash = murmur.end();
}

VertexLayout &VertexLayout::add(Attrib::Enum _attrib, uint8_t _num, AttribType::Enum _type, bool _normalized,
                                bool _asInt)
{
    const uint16_t encodedNorm = (_normalized & 1) << 7;
    const uint16_t encodedType = (_type & 7) << 3;
    const uint16_t encodedNum = (_num - 1) & 3;
    const uint16_t encodeAsInt = (_asInt & (!!"\x1\x1\x1\x0\x0"[_type])) << 8;
    m_attributes[_attrib] = encodedNorm | encodedType | encodedNum | encodeAsInt;

    m_offset[_attrib] = m_stride;
    m_stride += (*s_attribTypeSize[m_hash])[_type][_num - 1];

    return *this;
}

VertexLayout &VertexLayout::skip(uint8_t _num)
{
    m_stride += _num;

    return *this;
}

void VertexLayout::decode(Attrib::Enum _attrib, uint8_t &_num, AttribType::Enum &_type, bool &_normalized,
                          bool &_asInt) const
{
    uint16_t val = m_attributes[_attrib];
    _num = (val & 3) + 1;
    _type = AttribType::Enum((val >> 3) & 7);
    _normalized = !!(val & (1 << 7));
    _asInt = !!(val & (1 << 8));
}

	static const char* s_attrName[] =
	{
		"P",  "Attrib::Position",
		"N",  "Attrib::Normal",
		"T",  "Attrib::Tangent",
		"B",  "Attrib::Bitangent",
		"C0", "Attrib::Color0",
		"C1", "Attrib::Color1",
		"C2", "Attrib::Color2",
		"C3", "Attrib::Color3",
		"I",  "Attrib::Indices",
		"W",  "Attrib::Weights",
		"T0", "Attrib::TexCoord0",
		"T1", "Attrib::TexCoord1",
		"T2", "Attrib::TexCoord2",
		"T3", "Attrib::TexCoord3",
		"T4", "Attrib::TexCoord4",
		"T5", "Attrib::TexCoord5",
		"T6", "Attrib::TexCoord6",
		"T7", "Attrib::TexCoord7",
	};
	static_assert(BX_COUNTOF(s_attrName) == Attrib::Count*2);
    
	const char* getAttribName(Attrib::Enum _attr)
	{
		return s_attrName[_attr*2+1];
	}

} // namespace TinyRender
