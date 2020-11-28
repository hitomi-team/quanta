#include "pch/pch.h"

#include "openglrenderer.h"
#include "imgui_impl_opengl3.h"

static const GLenum glprimtypes[] = {
	GL_TRIANGLES,
	GL_LINES,
	GL_POINT,
	GL_TRIANGLE_STRIP,
	GL_LINE_STRIP,
	GL_TRIANGLE_FAN
};

#ifdef __DEBUG
static void gldbgcallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, const void *param)
{
	const char *source_str, *type_str, *severity_str;

#define STRTOENUM(a, b) case b: a = #b; break
	switch (source) {
	STRTOENUM(source_str, GL_DEBUG_SOURCE_API);
	STRTOENUM(source_str, GL_DEBUG_SOURCE_WINDOW_SYSTEM);
	STRTOENUM(source_str, GL_DEBUG_SOURCE_SHADER_COMPILER);
	STRTOENUM(source_str, GL_DEBUG_SOURCE_THIRD_PARTY);
	STRTOENUM(source_str, GL_DEBUG_SOURCE_APPLICATION);
	STRTOENUM(source_str, GL_DEBUG_SOURCE_OTHER);
	default: source_str = "unknown"; break;
	}

	switch (type) {
	STRTOENUM(type_str, GL_DEBUG_TYPE_ERROR);
	STRTOENUM(type_str, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR);
	STRTOENUM(type_str, GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR);
	STRTOENUM(type_str, GL_DEBUG_TYPE_PORTABILITY);
	STRTOENUM(type_str, GL_DEBUG_TYPE_PERFORMANCE);
	STRTOENUM(type_str, GL_DEBUG_TYPE_OTHER);
	STRTOENUM(type_str, GL_DEBUG_TYPE_MARKER);
	STRTOENUM(type_str, GL_DEBUG_TYPE_PUSH_GROUP);
	STRTOENUM(type_str, GL_DEBUG_TYPE_POP_GROUP);
	default: type_str = "unknown"; break;
	}

	switch (severity) {
	STRTOENUM(severity_str, GL_DEBUG_SEVERITY_HIGH);
	STRTOENUM(severity_str, GL_DEBUG_SEVERITY_MEDIUM);
	STRTOENUM(severity_str, GL_DEBUG_SEVERITY_LOW);
	STRTOENUM(severity_str, GL_DEBUG_SEVERITY_NOTIFICATION);
	default: severity_str = "unknown"; break;
	}

	(void)id; (void)length; (void)param;
	global_log.Debug(FMT_STRING("[GLDBG] [%s] [%s] [%s] %s"), source_str, type_str, severity_str, msg);
}
#endif

namespace Renderer {

	bool OpenGLRenderer::SetGraphicsMode(int width, int height, bool fullscreen, bool borderless, bool resizable, bool vsync, int multisample)
	{
		(void)vsync;
		(void)multisample;

		if (SDL_WasInit(SDL_INIT_VIDEO) != SDL_INIT_VIDEO)
			SDL_Init(SDL_INIT_VIDEO);

		int x = fullscreen ? 0 : SDL_WINDOWPOS_CENTERED;
		int y = fullscreen ? 0 : SDL_WINDOWPOS_CENTERED;

		unsigned flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

		if (fullscreen) {
			SDL_GetCurrentDisplayMode(0, &display);

			width = display.w;
			height = display.h;

			flags |= SDL_WINDOW_FULLSCREEN;
		} else if (resizable) {
			flags |= SDL_WINDOW_RESIZABLE;
		} else if (borderless) {
			flags |= SDL_WINDOW_BORDERLESS;
		}

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef __DEBUG
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#else
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
#endif

		this->window = SDL_CreateWindow(APP_TITLE, x, y, width, height, flags);
		if (this->window == nullptr) {
			FATAL(SDL_GetError());
			return false;
		}

		if ((this->rc = SDL_GL_CreateContext(this->window)) == nullptr) {
			FATAL(SDL_GetError());
			return false;
		}

		if (vsync)
			SDL_GL_SetSwapInterval(1);
		else
			SDL_GL_SetSwapInterval(0);

		gladLoadGLLoader(SDL_GL_GetProcAddress);

		IMGUI_CHECKVERSION();
		this->imguictx = ImGui::CreateContext();
		ImGui::SetCurrentContext(this->imguictx);
		ImGui::GetIO().DisplaySize.x = width;
		ImGui::GetIO().DisplaySize.y = height;
		ImGui::GetIO().Fonts->AddFontDefault();
		ImGui::GetIO().Fonts->Build();
		ImGui::StyleColorsDark();
		ImGui_ImplSDL2_InitForOpenGL(this->window, this->rc);
		ImGui_ImplOpenGL3_Init();

#ifdef __DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(gldbgcallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif

		global_log.Debug(FMT_STRING("Using OpenGL 4.6 Core\nGL_VENDOR: {}\nGL_RENDERER: {}\nGL_VERSION: {}"), glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

		return true;
	}

	bool OpenGLRenderer::BeginFrame()
	{
		if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
			return false;

		return true;
	}

	void OpenGLRenderer::EndFrame()
	{
		SDL_GL_SwapWindow(this->window);
	}

	void OpenGLRenderer::Clear(unsigned flags, const glm::vec4& color, float depth, unsigned stencil)
	{
		unsigned flags_color_clear = flags & CLEAR_COLOR;
		unsigned flags_depth_clear = flags & CLEAR_DEPTH;
		unsigned flags_stencil_clear = flags & CLEAR_STENCIL;

		GLenum clear_bits = \
			(flags_color_clear ? GL_COLOR_BUFFER_BIT : 0) | \
			(flags_depth_clear ? GL_DEPTH_BUFFER_BIT : 0) | \
			(flags_stencil_clear ? GL_STENCIL_BUFFER_BIT : 0);

		glClearColor(color.r, color.g, color.b, color.a);
		glClearDepth(depth);
		glClearStencil(stencil);
		glClear(clear_bits);
	}


	glm::vec2 OpenGLRenderer::GetRenderTargetDimensions()
	{
		return glm::vec2(0, 0);
	}

	void OpenGLRenderer::CreateRendererCapabilities()
	{
	}

	std::vector<int> OpenGLRenderer::GetMultiSampleLevels()
	{
		std::vector< int > a;
		return a;
	}


	void OpenGLRenderer::Close()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::SetCurrentContext(nullptr);
		ImGui::DestroyContext(this->imguictx);

		this->ResetCache();

		SDL_GL_MakeCurrent(nullptr, nullptr);
		SDL_GL_DeleteContext(this->rc);
		SDL_DestroyWindow(this->window);
		SDL_Quit();

		this->window = nullptr;
	}


	void OpenGLRenderer::AddGpuResource(GPUResource* object)
	{
		(void)object;
	}

	void OpenGLRenderer::RemoveGpuResource(GPUResource* object)
	{
		(void)object;
	}


	void OpenGLRenderer::ResetCache()
	{
	}


	bool OpenGLRenderer::IsDeviceLost()
	{
		return false;
	}


	VertexBuffer* OpenGLRenderer::CreateVertexBuffer(const Vertex *vertices, unsigned count)
	{
		(void)vertices;
		(void)count;
		return nullptr;
	}

	IndexBuffer* OpenGLRenderer::CreateIndexBuffer(const uint16_t *indices, unsigned count)
	{
		(void)indices;
		(void)count;
		return nullptr;
	}

	InputLayout* OpenGLRenderer::CreateInputLayout(unsigned char *vsbytecode, unsigned vsbytecodelen)
	{
		(void)vsbytecode;
		(void)vsbytecodelen;
		return nullptr;
	}

	Shader *OpenGLRenderer::CreateShader(unsigned char *vs_bytecode, unsigned int vs_size,
				     unsigned char *fs_bytecode, unsigned int fs_size)
	{
		(void)vs_bytecode;
		(void)vs_size;
		(void)fs_bytecode;
		(void)fs_size;
		return nullptr;
	}

	Texture2D *OpenGLRenderer::CreateTexture2D(unsigned char *data, unsigned width, unsigned height, SamplerStateDesc samplerstatedesc)
	{
		(void)data;
		(void)width;
		(void)height;
		(void)samplerstatedesc;
		return nullptr;
	}

	ShaderParameterBuffer *OpenGLRenderer::CreateShaderParameterBuffer(std::vector<ShaderParameterElement> elements)
	{
		(void)elements;
		return nullptr;
	}

	void OpenGLRenderer::SetShaders(Shader *shader)
	{
		(void)shader;
	}

	void OpenGLRenderer::SetVertexBuffer(VertexBuffer* buffer)
	{
		(void)buffer;
	}

	void OpenGLRenderer::SetIndexBuffer(IndexBuffer* buffer)
	{
		(void)buffer;
	}

	bool OpenGLRenderer::SetVertexBuffers(const std::vector<VertexBuffer*>& buffers, const std::vector<unsigned>& elementMasks, unsigned instanceOffset)
	{
		(void)buffers;
		(void)elementMasks;
		(void)instanceOffset;
		return true;
	}


	void OpenGLRenderer::SetFlushGPU(bool flushGpu)
	{
		(void)flushGpu;
	}

	void OpenGLRenderer::SetBlendMode(BlendMode mode)
	{
		(void)mode;
	}

	void OpenGLRenderer::SetColorWrite(bool enable)
	{
		(void)enable;
	}

	void OpenGLRenderer::SetCullMode(CullMode mode)
	{
		(void)mode;
	}

	void OpenGLRenderer::SetDepthBias(float constantBias, float slopeScaledBias)
	{
		(void)constantBias;
		(void)slopeScaledBias;
	}

	void OpenGLRenderer::SetDepthTest(CompareMode mode)
	{
		(void)mode;
	}

	void OpenGLRenderer::SetDepthWrite(bool enable)
	{
		(void)enable;
	}

	void OpenGLRenderer::SetFillMode(FillMode mode)
	{
		(void)mode;
	}

	void OpenGLRenderer::SetScissorTest(bool enable, const glm::vec2& rect)
	{
		(void)enable;
		(void)rect;
	}

	void OpenGLRenderer::SetStencilTest(bool enable, CompareMode mode, StencilOp pass, StencilOp fail, StencilOp zFail, unsigned stencilRef, unsigned compareMask, unsigned writeMask)
	{
		(void)enable;
		(void)mode;
		(void)pass;
		(void)fail;
		(void)zFail;
		(void)stencilRef;
		(void)compareMask;
		(void)writeMask;
	}


	void OpenGLRenderer::SetTexture(unsigned index, Texture2D* texture)
	{
		(void)index;
		(void)texture;
	}


	void OpenGLRenderer::SetRenderTarget(unsigned index, RenderTarget* renderTarget)
	{
		(void)index;
		(void)renderTarget;
	}

	void OpenGLRenderer::SetDepthStencil(RenderTarget* depthStencil)
	{
		(void)depthStencil;
	}


	void OpenGLRenderer::SetViewport(const glm::vec4& rect)
	{
		glViewport(rect.x, rect.y, rect.z, rect.w);
	}


	void OpenGLRenderer::Draw(PrimitiveType type, unsigned vertexStart, unsigned vertexCount)
	{
		glDrawArrays(glprimtypes[type], vertexStart, vertexCount);
	}

	void OpenGLRenderer::DrawIndexed(PrimitiveType type, unsigned indexStart, unsigned indexCount)
	{
		(void)type;
		(void)indexStart;
		(void)indexCount;
		glDrawElements(glprimtypes[type], indexCount, GL_UNSIGNED_INT, reinterpret_cast< const void * >(indexCount));
	}

	void OpenGLRenderer::DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned instanceCount)
	{
		(void)type;
		(void)indexStart;
		(void)indexCount;
		(void)instanceCount;
	}

	SDL_Window *OpenGLRenderer::GetWindow()
	{
		return this->window;
	}


	void OpenGLRenderer::ImGuiNewFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();
	}

	void OpenGLRenderer::ImGuiEndFrame()
	{
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	RendererType OpenGLRenderer::getRendererType()
	{
		return RENDERER_OPENGL;
	}

}
