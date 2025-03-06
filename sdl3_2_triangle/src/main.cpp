/******************************************************************************
* Based on tutorial:
* SDL 3 GPU tutorial with C++23 and Modern CMake
* https://www.youtube.com/watch?v=UFuWGECc8w0&t=5122s
******************************************************************************/

#include <SDL3/SDL.h>
#include <string>
#include <print>
#include <stdexcept>
#include <vector>
#include <fstream>

//#include <SDL3/SDL_main.h>

class SDLException final : public std::runtime_error {
    public:
        // VikingCat - What is explicit?
        inline explicit SDLException(const std::string &message) : std::runtime_error( message + "\n\t" + SDL_GetError()) {}
};

// static const char *BasePath{};
static std::string BasePath{};
void InitializeAssetLoader() {
    BasePath = std::string{SDL_GetBasePath()};
}

SDL_GPUShader* LoadShader(
    SDL_GPUDevice* device, 
    //const char* shaderFilename,
    const std::string &shaderFilename,
	const Uint32 samplerCount,
	const Uint32 uniformBufferCount,
	const Uint32 storageBufferCount,
	const Uint32 storageTextureCount
) {
	// Auto-detect the shader stage from the file name
	SDL_GPUShaderStage stage;
    if (shaderFilename.contains(".vert"))
		stage = SDL_GPU_SHADERSTAGE_VERTEX;
    else if (shaderFilename.contains(".frag"))
		stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
	else
        throw std::runtime_error("SDL Invalid Shader File");

	// char fullPath[256];
    std::string fullPath;
	const SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(device);
	SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
	const char *entrypoint;

	if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
		// SDL_snprintf(fullPath, sizeof(fullPath), "%sContent/Shaders/Compiled/SPIRV/%s.spv", BasePath, shaderFilename);
        // fullPath = BasePath + "Content/Shaders/Compiled/SPIRV/" + shaderFilename + ".spv";
        fullPath = std::format("{}/Content/Shaders/Compiled/SPIRV/{}.spv", BasePath, shaderFilename);
		format = SDL_GPU_SHADERFORMAT_SPIRV;
		entrypoint = "main";
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
		// SDL_snprintf(fullPath, sizeof(fullPath), "%sContent/Shaders/Compiled/MSL/%s.msl", BasePath, shaderFilename);
        fullPath = std::format("{}/Content/Shaders/Compiled/MSL/{}.msl", BasePath, shaderFilename);
		format = SDL_GPU_SHADERFORMAT_MSL;
		entrypoint = "main0";
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
		// SDL_snprintf(fullPath, sizeof(fullPath), "%sContent/Shaders/Compiled/DXIL/%s.dxil", BasePath, shaderFilename);
        fullPath = std::format("{}/Content/Shaders/Compiled/DXIL/{}.dxil", BasePath, shaderFilename);
		format = SDL_GPU_SHADERFORMAT_DXIL;
		entrypoint = "main";
	} else 
        throw std::runtime_error("SDL Shader Format Not Supported");

	/*size_t codeSize;
	void* code = SDL_LoadFile(fullPath, &codeSize);
	if (code == nullptr)
	{
		SDL_Log("Failed to load shader from disk! %s", fullPath);
		return nullptr;
	}*/

    /*std::vector<Uint8> code;
    std::ifstream file{fullPath, std::ios::binary | std::ios::ate};
    if(!file)
        throw std::runtime_error("SDL Failed To Load Shader From Disk");
    code.resize(file.tellg());
    file.seekg(0);
    file.read(reinterpret_cast<char *>(code.data()), static_cast<std::streamsize>(code.size()));*/

    std::ifstream file{fullPath, std::ios::binary};
    if(!file)
        throw std::runtime_error("SDL Failed To Load Shader From Disk");
    // std::string content{std::istreambuf_iterator(file), {}};
    // std::vector<Uint8> code(content.begin(),content.end());
    std::vector<Uint8> code{std::istreambuf_iterator(file), {}};

    SDL_GPUShaderCreateInfo shaderInfo{};
    // shaderInfo.code = static_cast<const Uint8 *>(code);
    shaderInfo.code = code.data();
    // shaderInfo.code_size = codeSize;
    shaderInfo.code_size = code.size();
    shaderInfo.entrypoint = entrypoint;
    shaderInfo.format = format;
    shaderInfo.stage = stage;
    shaderInfo.num_samplers = samplerCount;
    shaderInfo.num_uniform_buffers = uniformBufferCount;
    shaderInfo.num_storage_buffers = storageBufferCount;
    shaderInfo.num_storage_textures = storageTextureCount;

	SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shaderInfo);
	if (!shader)
        throw SDLException("SDL Failed To Create Shader");

	return shader;
}

int main() {
    // 1. Initialize SDL
    if(!SDL_Init(SDL_INIT_VIDEO)) 
        throw SDLException("SDL Initialization Failed");

    // 1. Initialize Base Path
    InitializeAssetLoader();

    // 2. Create window
    SDL_Window *window{SDL_CreateWindow("Compute Test", 800, 600, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE)};
    if(!window)
        throw SDLException("SDL Failed To Create Window");
    
    // 3. Create GPU Device
    SDL_GPUDevice *device{SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_DXIL, true, nullptr)};
    if(!device)
        throw SDLException("SDL Failed To Create GPU Device");

    std::print("INFO: GPU Device Driver = {}", SDL_GetGPUDeviceDriver(device));

    // 4. Create GPU Device
    if(!SDL_ClaimWindowForGPUDevice(device, window))
        throw SDLException("SDL GPU Device Failed To Claim Window");

    // 4.x Load the shaders
    SDL_GPUShader *vertexShader{LoadShader(device, "RawTriangle.vert", 0, 0, 0, 0)};
    if(!vertexShader)
        throw SDLException("SDL Could Not Load The Vertex Shader"); 

    SDL_GPUShader *fragmentShader{LoadShader(device, "SolidColor.frag", 0, 0, 0, 0)};
    if(!fragmentShader)
        throw SDLException("SDL Could Not Load The Fragment Shader"); 

    SDL_GPUColorTargetDescription colorTargetDescription{};
    colorTargetDescription.format = SDL_GetGPUSwapchainTextureFormat(device, window);
    std::vector colorTargetDescriptions{colorTargetDescription};
    
    SDL_GPUGraphicsPipelineTargetInfo targetInfo{};
    targetInfo.color_target_descriptions = colorTargetDescriptions.data();
    targetInfo.num_color_targets = colorTargetDescriptions.size();

    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.vertex_shader = vertexShader;
    pipelineCreateInfo.fragment_shader = fragmentShader;
    pipelineCreateInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineCreateInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    pipelineCreateInfo.target_info = targetInfo;
    SDL_GPUGraphicsPipeline *pipeline{SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo)};
    if(!pipeline)
        throw SDLException{"SDL Could Not Create GPU Pipeline"};

    SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, fragmentShader);

    // 5. Ready to show window
    SDL_ShowWindow(window);

    // 6. Main Event Loop
    bool isRunning{true};
    SDL_Event event;

    while (isRunning)
    {
        // 6.1 Check & act on events
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    isRunning = false;
                    break;
                default: 
                    break;
            }
        }

        // 6.2 Aquire command buffer
        // This can store multiple commands
        SDL_GPUCommandBuffer *commandBuffer{SDL_AcquireGPUCommandBuffer(device)};
        if(!commandBuffer)
            throw SDLException("SDL Device Failed To Aquire Command Buffer");
        
        // 6.3 Create SwapChainTexture (Render Target Texture?)
        SDL_GPUTexture *swapChainTexture;
        SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window, &swapChainTexture, nullptr, nullptr);
        if(swapChainTexture)
        {
            SDL_GPUColorTargetInfo colorTarget{};
            colorTarget.texture = swapChainTexture;
            colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;     // first operation to texture, clear it
            colorTarget.clear_color = SDL_FColor{0.5f, 0.0f, 0.0f, 1.0f};
            colorTarget.store_op = SDL_GPU_STOREOP_STORE;   // last operation to texture, save it
            // std::vector<SDL_GPUColorTargetInfo> colorTargets{colorTarget};
            // Compiler will figure out type on its own when initialized with one object
            std::vector colorTargets{colorTarget};

            // 6.4 BEGIN RENDER PASS
            // depth stencil buffer currently set to "nullptr" because are not doing 3D now
            SDL_GPURenderPass *renderPass{SDL_BeginGPURenderPass(commandBuffer, colorTargets.data(), colorTargets.size(), nullptr)};
            SDL_BindGPUGraphicsPipeline(renderPass, pipeline);
            
            SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

            // 6.5 END RENDER PASS
            SDL_EndGPURenderPass(renderPass);
        }

        if(!SDL_SubmitGPUCommandBuffer(commandBuffer))
            throw SDLException("SDL Could Not Send Command Buffer To GPU");
    }

    return EXIT_SUCCESS;
}