#ifndef RENDERER_DEFS_H
#define RENDERER_DEFS_H

namespace Renderer {

	enum RendererType
	{
		RENDERER_NULL = 0,
		RENDERER_D3D11,
		RENDERER_VULKAN
	};

	enum PrimitiveType
	{
		TRIANGLE_LIST = 0,
		LINE_LIST,
		POINT_LIST,
		TRIANGLE_STRIP,
		LINE_STRIP,
		TRIANGLE_FAN
	};

	enum BlendMode
	{
		BLEND_REPLACE = 0,
		BLEND_ADD,
		BLEND_MULTIPLY,
		BLEND_ALPHA,
		BLEND_ADDALPHA,
		BLEND_PREMULALPHA,
		BLEND_INVDESTALPHA,
		BLEND_SUBTRACT,
		BLEND_SUBTRACTALPHA,
		BLEND_MAX,
		MAX_BLENDMODES
	};


	enum CompareMode
	{
		CMP_ALWAYS = 0,
		CMP_EQUAL,
		CMP_NOTEQUAL,
		CMP_LESS,
		CMP_LESSEQUAL,
		CMP_GREATER,
		CMP_GREATEREQUAL,
		MAX_COMPAREMODES
	};


	enum CullMode
	{
		CULL_NONE = 0,
		CULL_CCW,
		CULL_CW,
		MAX_CULLMODES
	};


	enum FillMode
	{
		FILL_SOLID = 0,
		FILL_WIREFRAME,
		FILL_POINT
	};


	enum StencilOp
	{
		OP_KEEP = 0,
		OP_ZERO,
		OP_REF,
		OP_INCR,
		OP_DECR
	};


	enum LockState
	{
		LOCK_NONE = 0,
		LOCK_HARDWARE,
		LOCK_SHADOW,
		LOCK_SCRATCH
	};


	enum VertexElement
	{
		ELEMENT_POSITION = 0,
		ELEMENT_NORMAL,
		ELEMENT_COLOR,
		ELEMENT_TEXCOORD1,
		ELEMENT_TEXCOORD2,
		ELEMENT_TEXCOORD3,
		ELEMENT_CUBETEXCOORD1,
		ELEMENT_CUBETEXCOORD2,
		ELEMENT_TANGENT,
		ELEMENT_BLENDWEIGHTS,
		ELEMENT_BLENDINDICES,
		ELEMENT_INSTANCEMATRIX1,
		ELEMENT_INSTANCEMATRIX2,
		ELEMENT_INSTANCEMATRIX3,
		// Custom 32-bit integer object index. Due to API limitations, not supported on D3D9
		ELEMENT_OBJECTINDEX,
		MAX_VERTEX_ELEMENTS
	};


	enum TextureFilterMode
	{
		FILTER_NEAREST = 0,
		FILTER_BILINEAR,
		FILTER_TRILINEAR,
		FILTER_ANISOTROPIC,
		FILTER_DEFAULT,
		MAX_FILTERMODES
	};


	enum TextureAddressMode
	{
		ADDRESS_WRAP = 0,
		ADDRESS_MIRROR,
		ADDRESS_CLAMP,
		ADDRESS_BORDER,
		MAX_ADDRESSMODES
	};


	enum TextureCoordinate
	{
		COORD_U = 0,
		COORD_V,
		COORD_W,
		MAX_COORDS
	};


	enum TextureUsage
	{
		TEXTURE_STATIC = 0,
		TEXTURE_DYNAMIC,
		TEXTURE_RENDERTARGET,
		TEXTURE_DEPTHSTENCIL,
		TEXTURE_DEPTHSTENCIL_READONLY,
		TEXTURE_UAV
	};


	enum CubeMapFace
	{
		FACE_POSITIVE_X = 0,
		FACE_NEGATIVE_X,
		FACE_POSITIVE_Y,
		FACE_NEGATIVE_Y,
		FACE_POSITIVE_Z,
		FACE_NEGATIVE_Z,
		MAX_CUBEMAP_FACES
	};


	enum CubeMapLayout
	{
		CML_HORIZONTAL = 0,
		CML_HORIZONTALNVIDIA,
		CML_HORIZONTALCROSS,
		CML_VERTICALCROSS,
		CML_BLENDER
	};


	enum RenderSurfaceUpdateMode
	{
		SURFACE_MANUALUPDATE = 0,
		SURFACE_UPDATEVISIBLE,
		SURFACE_UPDATEALWAYS
	};


	enum ShaderType
	{
		VS = 0,
		FS,
		GS
	};


	enum ShaderParameterUsage {
		SHADER_PARAM_MVP = 0, // Model * Perspective
		SHADER_PARAM_TIME, // time since startup (in float)
		SHADER_PARAM_ALBEDO // Base Texture
	};

	enum TextureUnit
	{
		TU_DIFFUSE = 0,
		TU_ALBEDOBUFFER = 0,
		TU_NORMAL = 1,
		TU_NORMALBUFFER = 1,
		TU_SPECULAR = 2,
		TU_EMISSIVE = 3,
		TU_ENVIRONMENT = 4,
		TU_VOLUMEMAP = 5,
		TU_CUSTOM1 = 6,
		TU_CUSTOM2 = 7,
		TU_LIGHTRAMP = 8,
		TU_LIGHTSHAPE = 9,
		TU_SHADOWMAP = 10,
		TU_FACESELECT = 11,
		TU_INDIRECTION = 12,
		TU_DEPTHBUFFER = 13,
		TU_LIGHTBUFFER = 14,
		TU_ZONE = 15,
		MAX_MATERIAL_TEXTURE_UNITS = 8,
		MAX_TEXTURE_UNITS = 16
	};

	enum MaterialTextures
	{
		MT_DIFFUSE,
		MT_NORMAL,
		MT_SPECULAR,
		MT_ALPHA,
		MT_ROUGHNESS,
		MT_EMISSIVE,
	};

	enum RenderTargetInOut
	{
		RT_INPUT = 0x1,
		RT_OUTPUT = 0x2,
		RT_DEPTHSTENCIL = 0x4,
		RT_UAV = 0x8
	};

	struct RenderTargetDesc
	{
		unsigned Index;
		unsigned Width;
		unsigned Height;
		unsigned Depth;

		std::string Name;
		unsigned Format;
		unsigned ArraySize;
		unsigned Mips;

		TextureUsage Usage;


		unsigned Type;
		std::string ClearColor;
	};

	enum TextureComparisonFunction
	{
		TCF_NEVER = 0,
		TCF_EQUAL,
		TCF_NOTEQUAL,
		TCF_LESS,
		TCF_LESSEQUAL,
		TCF_GREATER,
		TCF_GREATEREQUAL,
		TCF_ALWAYS,
	};

	struct SamplerStateDesc
	{
		TextureFilterMode Filter;
		TextureAddressMode AddressModeU;
		TextureAddressMode AddressModeV;
		TextureAddressMode AddressModeW;

		TextureComparisonFunction ComparisonFunc;

		float MipLODBias;
		float MaxLOD;
		float MinLOD;
		unsigned MaxAniso;

		float BorderColor[4];
	};

	enum MiscRenderingFlags
	{
		RF_NODEPTHSTENCIL = 0x1,
		RF_NOBLENDSTATE = 0x2,
		RF_CLEAR = 0x4
	};

	struct GIParameters
	{
		float Scale;
		float LPVFlux;
		bool DebugView;
	};

	static const int QUALITY_LOW = 0;
	static const int QUALITY_MEDIUM = 1;
	static const int QUALITY_HIGH = 2;
	static const int QUALITY_MAX = 15;

	static const unsigned CLEAR_COLOR = 0x1;
	static const unsigned CLEAR_DEPTH = 0x2;
	static const unsigned CLEAR_STENCIL = 0x4;

	static const unsigned MASK_NONE = 0x0;
	static const unsigned MASK_POSITION = 0x1;
	static const unsigned MASK_NORMAL = 0x2;
	static const unsigned MASK_COLOR = 0x4;
	static const unsigned MASK_TEXCOORD1 = 0x8;
	static const unsigned MASK_TEXCOORD2 = 0x10;
	static const unsigned MASK_CUBETEXCOORD1 = 0x20;
	static const unsigned MASK_CUBETEXCOORD2 = 0x40;
	static const unsigned MASK_TANGENT = 0x80;
	static const unsigned MASK_BLENDWEIGHTS = 0x100;
	static const unsigned MASK_BLENDINDICES = 0x200;
	static const unsigned MASK_INSTANCEMATRIX1 = 0x400;
	static const unsigned MASK_INSTANCEMATRIX2 = 0x800;
	static const unsigned MASK_INSTANCEMATRIX3 = 0x1000;
	static const unsigned MASK_OBJECTINDEX = 0x2000;
	static const unsigned MASK_TEXCOORD3 = 0x4000;
	static const unsigned MASK_DEFAULT = 0xffffffff;
	static const unsigned NO_ELEMENT = 0xffffffff;

	static const int MAX_RENDERTARGETS = 1;
	static const int MAX_VERTEX_STREAMS = 4;
	static const int MAX_CONSTANT_REGISTERS = 256;

	static const int BITS_PER_COMPONENT = 8;

}

#endif
