#include "pch/pch.h"

#include "vulkantexture2d.h"
#include "vulkanctx.h"
#include "vulkanimage.h"

static const VkSamplerAddressMode vkaddressmode[] = {
	VK_SAMPLER_ADDRESS_MODE_REPEAT,
	VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
	VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
	VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
};

static const VkCompareOp vkcomparisonfunc[] = {
	VK_COMPARE_OP_NEVER,
	VK_COMPARE_OP_EQUAL,
	VK_COMPARE_OP_NOT_EQUAL,
	VK_COMPARE_OP_LESS,
	VK_COMPARE_OP_LESS_OR_EQUAL,
	VK_COMPARE_OP_GREATER,
	VK_COMPARE_OP_GREATER_OR_EQUAL,
	VK_COMPARE_OP_ALWAYS
};

bool CVulkanTexture2D::SetData(unsigned char *data, unsigned width, unsigned height, Renderer::SamplerStateDesc samplerstatedesc)
{
	CVulkanBuffer buf;
	CVulkanImage *image;

	uint64_t imgsize = 4*width*height;

	CVulkanBufferInitInfo initInfo = {};
	initInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	initInfo.reqMemFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	initInfo.prefMemFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
	initInfo.vmaUsage = VMA_MEMORY_USAGE_CPU_ONLY;

	buf.Setup(initInfo, imgsize);
	buf.Upload(data, imgsize, 0);

	image = new CVulkanImage;
	image->Setup(width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	image->TransitionLayout(0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
	image->CopyBufferToImage(buf);
	image->TransitionLayout(VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

	buf.Release();

	this->buffer = image;

	VkFilter min_filter, mag_filter;
	VkSamplerMipmapMode mipmap_mode;

	switch (samplerstatedesc.Filter) {
	case Renderer::FILTER_DEFAULT:
	case Renderer::FILTER_NEAREST:
		min_filter = VK_FILTER_NEAREST;
		mag_filter = VK_FILTER_NEAREST;
		mipmap_mode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		break;
	case Renderer::FILTER_BILINEAR:
		min_filter = VK_FILTER_LINEAR;
		mag_filter = VK_FILTER_LINEAR;
		mipmap_mode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		break;
	case Renderer::FILTER_TRILINEAR:
	case Renderer::FILTER_ANISOTROPIC:
		min_filter = VK_FILTER_LINEAR;
		mag_filter = VK_FILTER_LINEAR;
		mipmap_mode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		break;
	default:
		min_filter = VK_FILTER_NEAREST;
		mag_filter = VK_FILTER_NEAREST;
		mipmap_mode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		break;
	}

	if (samplerstatedesc.MaxAniso < 1)
		samplerstatedesc.MaxAniso = 1;

	VkSamplerCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	createInfo.magFilter = mag_filter;
	createInfo.minFilter = min_filter;
	createInfo.mipmapMode = mipmap_mode;
	createInfo.addressModeU = vkaddressmode[samplerstatedesc.AddressModeU];
	createInfo.addressModeV = vkaddressmode[samplerstatedesc.AddressModeV];
	createInfo.addressModeW = vkaddressmode[samplerstatedesc.AddressModeW];
	createInfo.mipLodBias = samplerstatedesc.MipLODBias;
	createInfo.anisotropyEnable = samplerstatedesc.MaxAniso == 1 ? VK_FALSE : VK_TRUE;
	createInfo.maxAnisotropy = samplerstatedesc.MaxAniso;
	createInfo.compareEnable = VK_TRUE;
	createInfo.compareOp = vkcomparisonfunc[samplerstatedesc.ComparisonFunc];
	createInfo.minLod = samplerstatedesc.MinLOD;
	createInfo.maxLod = samplerstatedesc.MaxLOD;

	this->sampler = new VkSampler;
	VK_ASSERT(vkCreateSampler(g_vulkanCtx->device, &createInfo, nullptr, reinterpret_cast< VkSampler * >(this->sampler)), "Failed to create VkSampler");

	return true;
}

void CVulkanTexture2D::Release()
{
	if (this->buffer == nullptr || this->sampler == nullptr)
		return;

	auto image = reinterpret_cast< CVulkanImage * >(this->buffer);
	auto sampler = reinterpret_cast< VkSampler * >(this->sampler)[0];

	vkDestroySampler(g_vulkanCtx->device, sampler, nullptr);
	image->Release();
	delete image;
	delete reinterpret_cast< VkSampler * >(this->sampler);

	this->buffer = nullptr;
	this->sampler = nullptr;
}
