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
	global_log.Debug(FMT_STRING("[GLDBG] [{}] [{}] [{}] {}"), source_str, type_str, severity_str, msg);
}
#endif

namespace Renderer {

	OpenGLRenderer *global_gl = nullptr;

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

		global_gl = this;

		this->ResetCache();

		return true;
	}

	void OpenGLRenderer::PreDraw()
	{
		if (this->rendertargets_dirty) {
			this->rendertargets_dirty = false;
			if (this->rendertargets[0] == nullptr)
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			else
				glBindFramebuffer(GL_FRAMEBUFFER, reinterpret_cast< GLuint * >(this->rendertargets[0]->getView())[0]);
		}

		if (this->textureviews_dirty) {
			this->textureviews_dirty = false;
			glActiveTexture(GL_TEXTURE0);
			glBindSampler(0, reinterpret_cast< GLuint * >(this->textureviews[0]->GetSampler())[0]);
			glBindTextureUnit(0, reinterpret_cast< GLuint * >(this->textureviews[0]->GetView())[0]);
		}

		if (this->rasterizerstate_dirty) {
			this->rasterizerstate_dirty = false;
		}
	}

	bool OpenGLRenderer::BeginFrame()
	{
		if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
			return false;

		this->PreDraw();

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
		size_t i;

		this->rendertargets_dirty = false;
		this->textureviews_dirty = false;
		this->rasterizerstate_dirty = false;

		this->current_shader = nullptr;
		this->current_vbo = nullptr;
		this->current_ibo = nullptr;

		for (i = 0; i < MAX_RENDERTARGETS; i++)
			this->rendertargets[i] = nullptr;

		for (i = 0; i < MAX_TEXTURE_UNITS; i++)
			this->textureviews[i] = nullptr;
	}


	bool OpenGLRenderer::IsDeviceLost()
	{
		return false;
	}


	VertexBuffer* OpenGLRenderer::CreateVertexBuffer(const Vertex *vertices, unsigned count)
	{
		if (vertices == nullptr || count == 0)
			return nullptr;

		COpenGLVertexBuffer *out = new COpenGLVertexBuffer;
		if (!out->SetData(vertices, count)) {
			delete out;
			return nullptr;
		}

		return out;
	}

	IndexBuffer* OpenGLRenderer::CreateIndexBuffer(const uint16_t *indices, unsigned count)
	{
		if (indices == nullptr || count == 0)
			return nullptr;

		COpenGLIndexBuffer *out = new COpenGLIndexBuffer;
		if (!out->SetData(indices, count)) {
			delete out;
			return nullptr;
		}

		return out;
	}

	InputLayout* OpenGLRenderer::CreateInputLayout(unsigned char *vsbytecode, unsigned vsbytecodelen)
	{
		COpenGLInputLayout *inputlayout = new COpenGLInputLayout;

		if (!inputlayout->Setup(vsbytecode, vsbytecodelen)) {
			delete inputlayout;
			return nullptr;
		}

		return inputlayout;
	}

	Shader *OpenGLRenderer::CreateShader(unsigned char *vs_bytecode, unsigned int vs_size,
				     unsigned char *fs_bytecode, unsigned int fs_size)
	{
		if (vs_bytecode == nullptr || vs_size == 0 || fs_bytecode == nullptr || fs_size == 0)
			return nullptr;

		COpenGLShader *shader = new COpenGLShader;
		if (!shader->Build(vs_bytecode, vs_size, fs_bytecode, fs_size)) {
			delete shader;
			return nullptr;
		}

		return shader;
	}

	Texture2D *OpenGLRenderer::CreateTexture2D(unsigned char *data, unsigned width, unsigned height, SamplerStateDesc samplerstatedesc)
	{
		if (data == nullptr || width == 0 || height == 0)
			return nullptr;

		COpenGLTexture2D *texture2d = new COpenGLTexture2D;
		if (!texture2d->SetData(data, width, height, samplerstatedesc)) {
			delete texture2d;
			return nullptr;
		}

		return texture2d;
	}

	ShaderParameterBuffer *OpenGLRenderer::CreateShaderParameterBuffer(std::vector<ShaderParameterElement> elements)
	{
		if (elements.size() == 0)
			return nullptr;

		COpenGLParameterBuffer *parambuffer = new COpenGLParameterBuffer;
		if (!parambuffer->Setup(elements)) {
			delete parambuffer;
			return nullptr;
		}

		return parambuffer;
	}

	void OpenGLRenderer::SetShaders(Shader *shader)
	{
		if (shader == nullptr)
			return;

		if (this->current_shader == shader)
			return;

		GLuint *pid = reinterpret_cast< GLuint * >(shader->GetProgram());
		if (pid == nullptr)
			return;

		glUseProgram(pid[0]);
		this->current_shader = shader;
	}

	void OpenGLRenderer::SetVertexBuffer(VertexBuffer* buffer)
	{
		GLuint vao = reinterpret_cast< GLuint * >(this->current_shader->GetInputLayout())[0];

		if (this->current_vbo != buffer) {
			this->current_vbo = buffer;

			if (this->current_vbo == nullptr) {
				glVertexArrayVertexBuffer(vao, 0, 0, 0, 0);
				glVertexArrayVertexBuffer(vao, 1, 0, 0, 0);
				glVertexArrayVertexBuffer(vao, 2, 0, 0, 0);
				glBindVertexArray(0);
			} else {
				glVertexArrayVertexBuffer(vao, 0, reinterpret_cast< GLuint * >(this->current_vbo->GetBuffer())[0], 0, sizeof(Renderer::Vertex));
				glVertexArrayVertexBuffer(vao, 1, reinterpret_cast< GLuint * >(this->current_vbo->GetBuffer())[0], sizeof(glm::vec3), sizeof(Renderer::Vertex));
				glVertexArrayVertexBuffer(vao, 2, reinterpret_cast< GLuint * >(this->current_vbo->GetBuffer())[0], sizeof(glm::vec3)*2, sizeof(Renderer::Vertex));
				glBindVertexArray(reinterpret_cast< GLuint * >(this->current_shader->GetInputLayout())[0]);
			}

		}
	}

	void OpenGLRenderer::SetIndexBuffer(IndexBuffer* buffer)
	{
		GLuint vao = reinterpret_cast< GLuint * >(this->current_shader->GetInputLayout())[0];

		if (this->current_ibo != buffer) {
			this->current_ibo = buffer;
			if (buffer != nullptr)
				glVertexArrayElementBuffer(vao, reinterpret_cast< GLuint * >(buffer->GetBuffer())[0]);
			else
				glVertexArrayElementBuffer(vao, 0);
		}
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
		if (index >= MAX_TEXTURE_UNITS || texture == nullptr)
			return;

		if (this->textureviews[index] != texture) {
			this->textureviews_dirty = true;
			this->textureviews[index] = texture;
		}
	}


	void OpenGLRenderer::SetRenderTarget(unsigned index, RenderTarget* renderTarget)
	{
		if (index >= MAX_RENDERTARGETS || renderTarget == nullptr)
			return;

		if (this->rendertargets[index] != renderTarget) {
			this->rendertargets_dirty = true;
			this->rendertargets[index] = renderTarget;
		}
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
		if (this->current_shader == nullptr || vertexCount == 0)
			return;

		this->PreDraw();

		glDrawArrays(glprimtypes[type], vertexStart, vertexCount);
	}

	void OpenGLRenderer::DrawIndexed(PrimitiveType type, unsigned indexStart, unsigned indexCount)
	{
		if (this->current_shader == nullptr || indexCount == 0)
			return;

		this->PreDraw();

		glDrawElements(glprimtypes[type], indexCount, GL_UNSIGNED_SHORT, (char *)0 + indexStart);
	}

	void OpenGLRenderer::DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned instanceCount)
	{
		if (this->current_shader == nullptr || indexCount == 0)
			return;

		this->PreDraw();

		glDrawElementsInstanced(glprimtypes[type], indexCount, GL_UNSIGNED_SHORT, (char *)0 + indexStart, instanceCount);
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
