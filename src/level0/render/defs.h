#ifndef LEVEL0_RENDER_DEFS_H
#define LEVEL0_RENDER_DEFS_H

// Renderer and Resources
enum ERendererType
{
	RENDERER_NULL = 0,
	RENDERER_VULKAN,
	RENDERER_D3D12,
};

enum EPhysicalDeviceType
{
	PHYSICAL_DEVICE_TYPE_OTHER = 0,
	PHYSICAL_DEVICE_TYPE_INTEGRATED,
	PHYSICAL_DEVICE_TYPE_DISCRETE,
	PHYSICAL_DEVICE_TYPE_VIRTUAL,
	PHYSICAL_DEVICE_TYPE_CPU,
};

enum EResourceMemoryUsage
{
	RESOURCE_MEMORY_USAGE_NONE = 0,

	// Usually done for Staging Transfer CPU->Device
	// * This is needed for platforms where the device is not fully mapped into memory
	// * For example, x86 without resizable PCI BAR enabled
	// * However, other platforms will most likely not have this limitation, as devices are memory mapped and the host has full access to them.
	// * Support for full memory mapped devices can be queried with IRenderPhysicalDevice->HasFullMemoryAccess()
	RESOURCE_MEMORY_USAGE_CPU_ONLY,

	// Usually done for Device->CPU, modify Buffer, then CPU->Device
	RESOURCE_MEMORY_USAGE_CPU_COPY,

	// Usually done for Animating Vertex Buffers, Uniform Buffers
	RESOURCE_MEMORY_USAGE_CPU_TO_DEVICE,

	// Fast Device Access, Usually Terrain and Textures, Constant Uniform Buffers
	RESOURCE_MEMORY_USAGE_DEVICE_ONLY,

	// Screen Capture (can just download memory using a staging buffer), HDR
	RESOURCE_MEMORY_USAGE_DEVICE_TO_CPU,

	MAX_RESOURCE_MEMORY_USAGE_ENUM
};

enum EDeviceQueue
{
	DEVICE_QUEUE_UNKNOWN = 0,
	DEVICE_QUEUE_GRAPHICS,
	DEVICE_QUEUE_COMPUTE,
	DEVICE_QUEUE_TRANSFER,
	MAX_DEVICE_QUEUE_ENUM
};

enum EResourceAccess
{
	RESOURCE_ACCESS_INDIRECT_COMMAND_READ = 0x00000001,
	RESOURCE_ACCESS_INDEX_READ = 0x00000002,
	RESOURCE_ACCESS_VERTEX_ATTRIBUTE_READ = 0x00000004,
	RESOURCE_ACCESS_UNIFORM_READ = 0x00000008,
	RESOURCE_ACCESS_INPUT_ATTACHMENT_READ = 0x00000010,
	RESOURCE_ACCESS_SHADER_READ = 0x00000020,
	RESOURCE_ACCESS_SHADER_WRITE = 0x00000040,
	RESOURCE_ACCESS_COLOR_ATTACHMENT_READ = 0x00000080,
	RESOURCE_ACCESS_COLOR_ATTACHMENT_WRITE = 0x00000100,
	RESOURCE_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ = 0x00000200,
	RESOURCE_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE = 0x00000400,
	RESOURCE_ACCESS_TRANSFER_READ = 0x00000800,
	RESOURCE_ACCESS_TRANSFER_WRITE = 0x00001000,
	RESOURCE_ACCESS_HOST_READ = 0x00002000,
	RESOURCE_ACCESS_HOST_WRITE = 0x00004000,
	RESOURCE_ACCESS_MEMORY_READ = 0x00008000,
	RESOURCE_ACCESS_MEMORY_WRITE = 0x00010000,
	RESOURCE_ACCESS_TRANSFORM_FEEDBACK_WRITE = 0x02000000,
	RESOURCE_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ = 0x04000000,
	RESOURCE_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE = 0x08000000,
	RESOURCE_ACCESS_CONDITIONAL_RENDERING_READ = 0x00100000,
	RESOURCE_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT = 0x00080000,
	RESOURCE_ACCESS_ACCELERATION_STRUCTURE_READ = 0x00200000,
	RESOURCE_ACCESS_ACCELERATION_STRUCTURE_WRITE = 0x00400000,
	RESOURCE_ACCESS_FRAGMENT_DENSITY_MAP_READ = 0x01000000,
	RESOURCE_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ = 0x00800000,
	RESOURCE_ACCESS_NONE = 0,
};

enum EBufferUsage
{
	BUFFER_USAGE_TRANSFER_SRC = 0x00000001,
	BUFFER_USAGE_TRANSFER_DST = 0x00000002,
	BUFFER_USAGE_UNIFORM_TEXEL_BUFFER = 0x00000004,
	BUFFER_USAGE_STORAGE_TEXEL_BUFFER = 0x00000008,
	BUFFER_USAGE_UNIFORM_BUFFER = 0x00000010,
	BUFFER_USAGE_STORAGE_BUFFER = 0x00000020,
	BUFFER_USAGE_INDEX_BUFFER = 0x00000040,
	BUFFER_USAGE_VERTEX_BUFFER = 0x00000080,
	BUFFER_USAGE_INDIRECT_BUFFER = 0x00000100,
	BUFFER_USAGE_SHADER_DEVICE_ADDRESS = 0x00020000,
	BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER = 0x00000800,
	BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER = 0x00001000,
	BUFFER_USAGE_CONDITIONAL_RENDERING = 0x00000200,
	BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY = 0x00080000,
	BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE = 0x00100000,
	BUFFER_USAGE_SHADER_BINDING_TABLE = 0x00000400,
	BUFFER_USAGE_RAY_TRACING = BUFFER_USAGE_SHADER_BINDING_TABLE,
	BUFFER_USAGE_NONE = 0
};

enum EImageAspect
{
	IMAGE_ASPECT_COLOR = 0,
	IMAGE_ASPECT_DEPTH,
	IMAGE_ASPECT_DEPTH_STENCIL,
	IMAGE_ASPECT_STENCIL,
	MAX_IMAGE_ASPECT_ENUM
};

enum EImageFormat
{
	IMAGE_FORMAT_UNKNOWN = 0,

	IMAGE_FORMAT_R_UINT_8,
	IMAGE_FORMAT_RG_UINT_8_8_8,
	IMAGE_FORMAT_RGB_UINT_8_8_8,
	IMAGE_FORMAT_RGBA_UINT_8_8_8_8,
	IMAGE_FORMAT_RGBA_UINT_8_8_8_8_SRGB,
	IMAGE_FORMAT_RGBX_UINT_8_8_8_8,
	IMAGE_FORMAT_RGBX_UINT_8_8_8_8_SRGB,

	IMAGE_FORMAT_R_FLOAT_16,
	IMAGE_FORMAT_R_FLOAT_32,
	IMAGE_FORMAT_RG_FLOAT_16_16,
	IMAGE_FORMAT_RG_FLOAT_32_32,
	IMAGE_FORMAT_RGB_FLOAT_16_16_16,
	IMAGE_FORMAT_RGB_FLOAT_32_32_32,
	IMAGE_FORMAT_RGBA_FLOAT_16_16_16_16,
	IMAGE_FORMAT_RGBA_FLOAT_32_32_32_32,
	IMAGE_FORMAT_RGBX_FLOAT_16_16_16_16,
	IMAGE_FORMAT_RGBX_FLOAT_32_32_32_32,

	IMAGE_FORMAT_BGR_UINT_8_8_8,
	IMAGE_FORMAT_BGRA_UINT_8_8_8_8,
	IMAGE_FORMAT_BGRA_UINT_8_8_8_8_SRGB,
	IMAGE_FORMAT_BGRX_UINT_8_8_8_8,
	IMAGE_FORMAT_BGRX_UINT_8_8_8_8_SRGB,

	IMAGE_FORMAT_D_UINT_16,
	IMAGE_FORMAT_D_FLOAT_32,
	IMAGE_FORMAT_S_UINT_8,
	IMAGE_FORMAT_D_S_UINT_16_UINT_8,
	IMAGE_FORMAT_D_S_FLOAT_24_UINT_8_PACK_32,
	IMAGE_FORMAT_D_X_FLOAT_24_UINT_8_PACK_32,
	IMAGE_FORMAT_D_S_FLOAT_32_UINT_8,

	MAX_IMAGE_FORMAT_ENUM
};

enum EImageLayout
{
	IMAGE_LAYOUT_UNDEFINED = 0,
	IMAGE_LAYOUT_GENERAL,
	IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
	IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
	IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
	IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	IMAGE_LAYOUT_PREINITIALIZED,
	IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
	IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
	IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
	IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
	IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
	IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL,
	IMAGE_LAYOUT_PRESENT_SRC,
	IMAGE_LAYOUT_SHARED_PRESENT,
	IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL,
	IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL,
	IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
	IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,

	MAX_IMAGE_LAYOUT_ENUM
};

enum EImageType
{
	IMAGE_TYPE_1D = 0,
	IMAGE_TYPE_2D,
	IMAGE_TYPE_3D,
	IMAGE_TYPE_CUBEMAP,
	MAX_IMAGE_TYPE_ENUM
};

enum EImageUsage
{
	IMAGE_USAGE_TRANSFER_SRC = 0x00000001,
	IMAGE_USAGE_TRANSFER_DST = 0x00000002,
	IMAGE_USAGE_SAMPLED = 0x00000004,
	IMAGE_USAGE_STORAGE = 0x00000008,
	IMAGE_USAGE_COLOR_ATTACHMENT = 0x00000010,
	IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT = 0x00000020,
	IMAGE_USAGE_TRANSIENT_ATTACHMENT = 0x00000040,
	IMAGE_USAGE_INPUT_ATTACHMENT = 0x00000080,
	IMAGE_USAGE_FRAGMENT_DENSITY_MAP = 0x00000200,
	IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT = 0x00000100
};

enum ECommandPoolUsage
{
	COMMAND_POOL_USAGE_NORMAL = 0,
	COMMAND_POOL_USAGE_TRANSIENT = 0x00000001,
	COMMAND_POOL_USAGE_RESET_COMMAND_BUFFER = 0x00000002,
	COMMAND_POOL_USAGE_PROTECTED = 0x00000004,
	COMMAND_POOL_USAGE_ALL = 0x7FFFFFFF
};

enum ECommandBufferLevel
{
	COMMAND_BUFFER_LEVEL_PRIMARY,
	COMMAND_BUFFER_LEVEL_SECONDARY,
	MAX_COMMAND_BUFFER_LEVEL_ENUM
};

enum ECommandBufferUsage
{
	COMMAND_BUFFER_USAGE_NORMAL = 0,
	COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT = 0x00000001,
	COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE = 0x00000002,
	COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE = 0x00000004,
	COMMAND_BUFFER_USAGE_ALL = 0x7FFFFFFF
};

enum EDependencyFlag
{
	DEPENDENCY_BY_REGION = 0x00000001,
	DEPENDENCY_DEVICE_GROUP = 0x00000004,
	DEPENDENCY_VIEW_LOCAL = 0x00000002,
};

enum EDescriptorType
{
	DESCRIPTOR_TYPE_SAMPLER = 0,
	DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
	DESCRIPTOR_TYPE_SAMPLED_IMAGE,
	DESCRIPTOR_TYPE_STORAGE_IMAGE,
	DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
	DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
	DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	DESCRIPTOR_TYPE_STORAGE_BUFFER,
	DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
	DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
	DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
	DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK,
	DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE,
	MAX_DESCRIPTOR_TYPE_ENUM
};

enum EAttachmentLoadOp
{
	ATTACHMENT_LOAD_OP_LOAD = 0,
	ATTACHMENT_LOAD_OP_CLEAR,
	ATTACHMENT_LOAD_OP_DONT_CARE,
	ATTACHMENT_LOAD_OP_NONE,
	MAX_ATTACHMENT_LOAD_OP_ENUM
};

enum EAttachmentStoreOp
{
	ATTACHMENT_STORE_OP_STORE = 0,
	ATTACHMENT_STORE_OP_DONT_CARE,
	ATTACHMENT_STORE_OP_NONE,
	MAX_ATTACHMENT_STORE_OP_ENUM
};

enum EColorComponent
{
	COLOR_COMPONENT_R = 0x00000001,
	COLOR_COMPONENT_G = 0x00000002,
	COLOR_COMPONENT_B = 0x00000004,
	COLOR_COMPONENT_A = 0x00000008
};

enum ESwapchainPresentMode
{
	PRESENT_MODE_IMMEDIATE = 0,
	PRESENT_MODE_FIFO_VSYNC,
	PRESENT_MODE_FIFO_VSYNC_RELAXED,
	PRESENT_MODE_MAILBOX,
	MAX_PRESENT_MODE_ENUM
};

enum ESubpassContents
{
	SUBPASS_CONTENTS_INLINE = 0,
	SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS,
	MAX_SUBPASS_CONTENTS_ENUM
};

// Graphics/Compute Pipeline
enum EPipelineUsage
{
	PIPELINE_USAGE_DISABLE_OPTIMIZATION = 0x00000001,
	PIPELINE_USAGE_ALLOW_DERIVATIVES = 0x00000002,
	PIPELINE_USAGE_DERIVATIVE = 0x00000004,
	PIPELINE_USAGE_VIEW_INDEX_FROM_DEVICE_INDEX = 0x00000008,
	PIPELINE_USAGE_DISPATCH_BASE = 0x00000010,
	PIPELINE_USAGE_RASTERIZATION_STATE_CREATE_FRAGMENT_SHADING_RATE_ATTACHMENT = 0x00200000,
	PIPELINE_USAGE_RASTERIZATION_STATE_CREATE_FRAGMENT_DENSITY_MAP_ATTACHMENT = 0x00400000,
	PIPELINE_USAGE_RAY_TRACING_NO_NULL_ANY_HIT_SHADERS = 0x00004000,
	PIPELINE_USAGE_RAY_TRACING_NO_NULL_CLOSEST_HIT_SHADERS = 0x00008000,
	PIPELINE_USAGE_RAY_TRACING_NO_NULL_MISS_SHADERS = 0x00010000,
	PIPELINE_USAGE_RAY_TRACING_NO_NULL_INTERSECTION_SHADERS = 0x00020000,
	PIPELINE_USAGE_RAY_TRACING_SKIP_TRIANGLES = 0x00001000,
	PIPELINE_USAGE_RAY_TRACING_SKIP_AABBS = 0x00002000,
	PIPELINE_USAGE_RAY_TRACING_SHADER_GROUP_HANDLE_CAPTURE_REPLAY = 0x00080000,
	PIPELINE_USAGE_CAPTURE_STATISTICS = 0x00000040,
	PIPELINE_USAGE_CAPTURE_INTERNAL_REPRESENTATIONS = 0x00000080,
	PIPELINE_USAGE_LIBRARY = 0x00000800,
	PIPELINE_USAGE_FAIL_ON_PIPELINE_COMPILE_REQUIRED = 0x00000100,
	PIPELINE_USAGE_EARLY_RETURN_ON_FAILURE = 0x00000200
};

enum EPipelineStage
{
	PIPELINE_STAGE_TOP_OF_PIPE = 0x00000001,
	PIPELINE_STAGE_DRAW_INDIRECT = 0x00000002,
	PIPELINE_STAGE_VERTEX_INPUT = 0x00000004,
	PIPELINE_STAGE_VERTEX_SHADER = 0x00000008,
	PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER = 0x00000010,
	PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER = 0x00000020,
	PIPELINE_STAGE_GEOMETRY_SHADER = 0x00000040,
	PIPELINE_STAGE_FRAGMENT_SHADER = 0x00000080,
	PIPELINE_STAGE_EARLY_FRAGMENT_TESTS = 0x00000100,
	PIPELINE_STAGE_LATE_FRAGMENT_TESTS = 0x00000200,
	PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT = 0x00000400,
	PIPELINE_STAGE_COMPUTE_SHADER = 0x00000800,
	PIPELINE_STAGE_TRANSFER = 0x00001000,
	PIPELINE_STAGE_BOTTOM_OF_PIPE = 0x00002000,
	PIPELINE_STAGE_HOST = 0x00004000,
	PIPELINE_STAGE_ALL_GRAPHICS = 0x00008000,
	PIPELINE_STAGE_ALL_COMMANDS = 0x00010000,
};

enum EShaderStage
{
	SHADER_STAGE_VERTEX = 0x00000001,
	SHADER_STAGE_TESSELLATION_CONTROL = 0x00000002,
	SHADER_STAGE_TESSELLATION_EVALUATION = 0x00000004,
	SHADER_STAGE_GEOMETRY = 0x00000008,
	SHADER_STAGE_FRAGMENT = 0x00000010,
	SHADER_STAGE_COMPUTE = 0x00000020,
	SHADER_STAGE_ALL_GRAPHICS = 0x0000001F,
	SHADER_STAGE_ALL = 0x7FFFFFFF,
	SHADER_STAGE_RAYTRACING_GEN = 0x00000100,
	SHADER_STAGE_RAYTRACING_ANY_HIT = 0x00000200,
	SHADER_STAGE_RAYTRACING_CLOSEST_HIT = 0x00000400,
	SHADER_STAGE_RAYTRACING_MISS = 0x00000800,
	SHADER_STAGE_RAYTRACING_INTERSECTION = 0x00001000,
	SHADER_STAGE_RAYTRACING_CALLABLE = 0x00002000,
};

enum EPrimitiveType
{
	PRIMITIVE_TYPE_TRIANGLE_LIST = 0,
	PRIMITIVE_TYPE_LINE_LIST,
	PRIMITIVE_TYPE_POINT_LIST,
	PRIMITIVE_TYPE_TRIANGLE_STRIP,
	PRIMITIVE_TYPE_LINE_STRIP,
	PRIMITIVE_TYPE_TRIANGLE_FAN,
	MAX_PRIMITIVE_TYPE_ENUM
};

enum EBlendOp {
	BLEND_OP_ADD = 0,
	BLEND_OP_SUBTRACT,
	BLEND_OP_REVERSE_SUBTRACT,
	BLEND_OP_MIN,
	BLEND_OP_MAX,
	BLEND_OP_ZERO,
	BLEND_OP_SRC,
	BLEND_OP_DST,
	BLEND_OP_SRC_OVER,
	BLEND_OP_DST_OVER,
	BLEND_OP_SRC_IN,
	BLEND_OP_DST_IN,
	BLEND_OP_SRC_OUT,
	BLEND_OP_DST_OUT,
	BLEND_OP_SRC_ATOP,
	BLEND_OP_DST_ATOP,
	BLEND_OP_XOR,
	BLEND_OP_MULTIPLY,
	BLEND_OP_SCREEN,
	BLEND_OP_OVERLAY,
	BLEND_OP_DARKEN,
	BLEND_OP_LIGHTEN,
	BLEND_OP_COLORDODGE,
	BLEND_OP_COLORBURN,
	BLEND_OP_HARDLIGHT,
	BLEND_OP_SOFTLIGHT,
	BLEND_OP_DIFFERENCE,
	BLEND_OP_EXCLUSION,
	BLEND_OP_INVERT,
	BLEND_OP_INVERT_RGB,
	BLEND_OP_LINEARDODGE,
	BLEND_OP_LINEARBURN,
	BLEND_OP_VIVIDLIGHT,
	BLEND_OP_LINEARLIGHT,
	BLEND_OP_PINLIGHT,
	BLEND_OP_HARDMIX,
	BLEND_OP_HSL_HUE,
	BLEND_OP_HSL_SATURATION,
	BLEND_OP_HSL_COLOR,
	BLEND_OP_HSL_LUMINOSITY,
	BLEND_OP_PLUS,
	BLEND_OP_PLUS_CLAMPED,
	BLEND_OP_PLUS_CLAMPED_ALPHA,
	BLEND_OP_PLUS_DARKER,
	BLEND_OP_MINUS,
	BLEND_OP_MINUS_CLAMPED,
	BLEND_OP_CONTRAST,
	BLEND_OP_INVERT_OVG,
	BLEND_OP_RED,
	BLEND_OP_GREEN,
	BLEND_OP_BLUE,
	MAX_BLEND_OPS,
};

enum EBlendFactor {
	BLEND_FACTOR_ZERO = 0,
	BLEND_FACTOR_ONE,
	BLEND_FACTOR_SRC_COLOR,
	BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
	BLEND_FACTOR_DST_COLOR,
	BLEND_FACTOR_ONE_MINUS_DST_COLOR,
	BLEND_FACTOR_SRC_ALPHA,
	BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
	BLEND_FACTOR_DST_ALPHA,
	BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
	BLEND_FACTOR_CONSTANT_COLOR,
	BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
	BLEND_FACTOR_CONSTANT_ALPHA,
	BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
	BLEND_FACTOR_SRC_ALPHA_SATURATE,
	BLEND_FACTOR_SRC1_COLOR,
	BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,
	BLEND_FACTOR_SRC1_ALPHA,
	BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA,
	MAX_BLEND_FACTORS
};

enum ECullMode
{
	CULL_NONE = 0,
	CULL_FRONT,
	CULL_BACK,
	CULL_FRONT_AND_BACK,
	MAX_CULLMODES
};

enum EFrontFace
{
	FRONT_FACE_CCW = 0,
	FRONT_FACE_CW,
	MAX_FRONT_FACE_ENUM
};

enum EFillMode
{
	FILL_MODE_SOLID = 0,
	FILL_MODE_WIREFRAME,
	FILL_MODE_POINT,
	MAX_FILL_MODE_ENUM
};

enum EStencilOp
{
	STENCIL_OP_KEEP = 0,
	STENCIL_OP_ZERO,
	STENCIL_OP_REPLACE,
	STENCIL_OP_INCR_CLAMP,
	STENCIL_OP_DECR_CLAMP,
	STENCIL_OP_INVERT,
	STENCIL_OP_INCR_WRAP,
	STENCIL_OP_DECR_WRAP,
	MAX_STENCIL_OP_ENUM
};

enum ELogicOp
{
	LOGIC_OP_CLEAR = 0,
	LOGIC_OP_AND,
	LOGIC_OP_AND_REVERSE,
	LOGIC_OP_COPY,
	LOGIC_OP_AND_INVERTED,
	LOGIC_OP_NO_OP,
	LOGIC_OP_XOR,
	LOGIC_OP_OR,
	LOGIC_OP_NOR,
	LOGIC_OP_EQUIVALENT,
	LOGIC_OP_INVERT,
	LOGIC_OP_OR_REVERSE,
	LOGIC_OP_COPY_INVERTED,
	LOGIC_OP_OR_INVERTED,
	LOGIC_OP_NAND,
	LOGIC_OP_SET,
	MAX_LOGIC_OP_ENUM
};

enum EVertexInputRate
{
	VERTEX_INPUT_RATE_VERTEX = 0,
	VERTEX_INPUT_RATE_INSTANCE,
	MAX_VERTEX_INPUT_RATE_ENUM
};

enum EShaderType
{
	SHADER_TYPE_VERTEX = 0,
	SHADER_TYPE_FRAGMENT,
	SHADER_TYPE_GEOMETRY,
	SHADER_TYPE_TESSELLATION_CONTROL,
	SHADER_TYPE_TESSELLATION_EVALUATION,
	MAX_SHADER_TYPES
};

// We might do raytracing at some point
enum ERaytracingShaderType {
	RAYTRACING_SHADER_TYPE_GEN = 0,
	RAYTRACING_SHADER_TYPE_HIT,
	RAYTRACING_SHADER_TYPE_CLOSEST,
	RAYTRACING_SHADER_TYPE_MISS,
	RAYTRACING_SHADER_TYPE_INTERSECTION,
	RAYTRACING_SHADER_TYPE_CALLABLE,
	MAX_RAYTRACING_SHADER_TYPES
};

// Locking
enum ELockState
{
	LOCK_NONE = 0,
	LOCK_HARDWARE,
	LOCK_SHADOW,
	LOCK_SCRATCH
};

// Vertex Type
enum EVertexElement
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

// Texture Sampling
enum ETextureFilterMode
{
	FILTER_NEAREST = 0,
	FILTER_BILINEAR,
	FILTER_TRILINEAR,
	FILTER_ANISOTROPIC,
	FILTER_DEFAULT,
	MAX_FILTERMODES
};

enum ETextureAddressMode
{
	ADDRESS_MODE_REPEAT = 0,
	ADDRESS_MODE_MIRROR_REPEAT,
	ADDRESS_MODE_CLAMP_TO_EDGE,
	ADDRESS_MODE_CLAMP_TO_BORDER,
	ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE,
	MAX_ADDRESSMODES
};

enum ETextureCoordinate
{
	COORD_U = 0,
	COORD_V,
	COORD_W,
	MAX_COORDS
};

enum ETextureUsage
{
	TEXTURE_STATIC = 0,
	TEXTURE_DYNAMIC,
	TEXTURE_RENDERTARGET,
	TEXTURE_DEPTHSTENCIL,
	TEXTURE_DEPTHSTENCIL_READONLY,
	TEXTURE_UAV
};

enum ECubeMapFace
{
	FACE_POSITIVE_X = 0,
	FACE_NEGATIVE_X,
	FACE_POSITIVE_Y,
	FACE_NEGATIVE_Y,
	FACE_POSITIVE_Z,
	FACE_NEGATIVE_Z,
	MAX_CUBEMAP_FACES
};

enum ECubeMapLayout
{
	CML_HORIZONTAL = 0,
	CML_HORIZONTALNVIDIA,
	CML_HORIZONTALCROSS,
	CML_VERTICALCROSS,
	CML_BLENDER
};

enum ETextureUnit
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

enum EMaterialTextures
{
	MT_DIFFUSE,
	MT_NORMAL,
	MT_SPECULAR,
	MT_ALPHA,
	MT_ROUGHNESS,
	MT_EMISSIVE,
};

enum ERenderSurfaceUpdateMode
{
	SURFACE_MANUALUPDATE = 0,
	SURFACE_UPDATEVISIBLE,
	SURFACE_UPDATEALWAYS
};

enum ETextureComparisonFunction
{
	TCF_NEVER = 0,
	TCF_EQUAL,
	TCF_NOTEQUAL,
	TCF_LESS,
	TCF_LESSEQUAL,
	TCF_GREATER,
	TCF_GREATEREQUAL,
	TCF_ALWAYS,
	MAX_TCF
};

enum ERenderTargetInOut
{
	RT_INPUT = 0x1,
	RT_OUTPUT = 0x2,
	RT_DEPTHSTENCIL = 0x4,
	RT_UAV = 0x8
};

enum EMiscRenderingFlags
{
	RF_NODEPTHSTENCIL = 0x1,
	RF_NOBLENDSTATE = 0x2,
	RF_CLEAR = 0x4
};

struct EGIParameters
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

static const int MAX_RENDERTARGETS = 6;
static const int MAX_VERTEX_STREAMS = 4;
static const int MAX_CONSTANT_REGISTERS = 256;

static const int BITS_PER_COMPONENT = 8;

#endif