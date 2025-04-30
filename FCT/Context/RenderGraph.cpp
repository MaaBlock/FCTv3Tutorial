#include "../FCTAPI.h"
namespace FCT
{
    RenderGraph::RenderGraph(Context* ctx): m_ctx(ctx)
    {

    }

    void RenderGraph::addPass(const std::string& name, Pass* pass)
    {
        PassGraphVertex v;
        v.name = name;
        v.pass = pass;
        PassGraphType::vertex_descriptor vd = boost::add_vertex(v, m_passGraph);
        m_passVertex[name] = vd;
    }

    void RenderGraph::addWindowResource(Window* wnd)
    {
        std::string name = "WndBackBuffer" + std::to_string(m_unnameWndBackBufferCount++);
        WindowImageResource res;
        res.wnd = wnd;
        res.img = wnd->getCurrentTarget()->targetImage();
        m_windowImageResources[name] = res;
        Image* img = res.img;
        img->addRef();
        m_images[name] = img;
        m_windowBackBufferNames[wnd] = name;
        if (wnd->getCurrentTarget()->depthStencilBuffer()) {
            std::string name = "WndDepthStencil" + std::to_string(m_unnameWndStencilDepthCount);
            WindowDepthStencilResource depthRes;
            depthRes.wnd = wnd;
            depthRes.img = wnd->getCurrentTarget()->depthStencilBuffer();
            m_windowDepthStencilResources[name] = depthRes;
            m_windowDepthStencilNames[wnd] = name;
            m_images[name] = depthRes.img;
            m_images[name]->addRef();
        }
    }

    void RenderGraph::submit(Job* job, std::string name)
    {
        PassGraphType::vertex_descriptor vd = m_passVertex[name];
        Pass* pass = m_passGraph[vd].pass;
        pass->submit(job);
    }

    void RenderGraph::addPassDenpendency(const std::string& from, const std::string& to)
    {
        PassGraphEdge e;
        e.type = PassGraphEdgeType::Dependency;
        PassGraphType::edge_descriptor ed = boost::add_edge(m_passVertex[from], m_passVertex[to], e, m_passGraph).first;
    }

    void RenderGraph::bindOutputImage(const std::string& name, std::string image, uint8_t slot)
    {
        ImageResourceDesc& desc = m_imageResourceDescs[image];
        desc.usage |= ImageUsage::RenderTarget;
        m_passGraph[m_passVertex[name]].target[slot] = image;
    }

    void RenderGraph::bindTextureImage(const char* name, std::string image, uint32_t width, uint32_t height,
        Samples samples)
    {
        ImageResourceDesc& desc = m_imageResourceDescs[image];
        desc.width |= width;
        desc.height |= height;
        desc.dynamicSize = false;
        if (samples != Samples::sample_undefined)
            desc.samples = samples;
        desc.usage |= ImageUsage::Texture;
        m_passGraph[m_passVertex[name]].textures.push_back(image);
    }

    void RenderGraph::bindDepthStencil(const char* name, std::string image)
    {
        ImageResourceDesc& desc = m_imageResourceDescs[image];
        desc.usage |= ImageUsage::DepthStencil;
        m_passGraph[m_passVertex[name]].depthStencil = image;
    }

    void RenderGraph::bindOutput(const char* name, Window* wnd, uint8_t slot)
    {
        if (m_windowBackBufferNames.find(wnd) != m_windowBackBufferNames.end())
        {
            bindOutputImage(name, m_windowBackBufferNames[wnd], slot);
        }
        if (m_windowDepthStencilNames.find(wnd)!= m_windowDepthStencilNames.end())
        {
            bindDepthStencil(name, m_windowDepthStencilNames[wnd]);
        }
    }

    void RenderGraph::checkAndUpdateResourceSizes()
    {
        bool needRecompute = false;

        for (const auto& entry : m_windowImageResources) {
            const std::string& imageName = entry.first;
            const WindowImageResource& resource = entry.second;

            uint32_t currentWidth = resource.wnd->getWidth();
            uint32_t currentHeight = resource.wnd->getHeight();

            if (m_imageResourceDescs[imageName].width != currentWidth ||
                m_imageResourceDescs[imageName].height != currentHeight) {
                m_imageResourceDescs[imageName].width = currentWidth;
                m_imageResourceDescs[imageName].height = currentHeight;
                needRecompute = true;
            }
        }

        for (const auto& entry : m_windowDepthStencilResources) {
            const std::string& imageName = entry.first;
            const WindowDepthStencilResource& resource = entry.second;

            uint32_t currentWidth = resource.wnd->getWidth();
            uint32_t currentHeight = resource.wnd->getHeight();

            if (m_imageResourceDescs[imageName].width != currentWidth ||
                m_imageResourceDescs[imageName].height != currentHeight) {
                m_imageResourceDescs[imageName].width = currentWidth;
                m_imageResourceDescs[imageName].height = currentHeight;
                needRecompute = true;
            }
        }

        if (needRecompute) {
            std::unordered_map<std::string, std::pair<uint32_t, uint32_t>> originalSizes;
            for (const auto& entry : m_imageResourceDescs) {
                if (entry.second.dynamicSize) {
                    originalSizes[entry.first] = {entry.second.width, entry.second.height};
                }
            }

            for (auto& entry : m_imageResourceDescs) {
                if (entry.second.dynamicSize && !entry.second.isWnd) {
                    entry.second.width = 0;
                    entry.second.height = 0;
                }
            }

            computeSize();

            for (const auto& entry : m_imageResourceDescs) {
                const std::string& imageName = entry.first;
                const ImageResourceDesc& desc = entry.second;

                if (m_windowImageResources.find(imageName) != m_windowImageResources.end() ||
                    m_windowDepthStencilResources.find(imageName) != m_windowDepthStencilResources.end()) {
                    continue;
                }

                auto imgIt = m_images.find(imageName);
                if (imgIt != m_images.end()) {
                    Image* img = imgIt->second;
                    auto originalSizeIt = originalSizes.find(imageName);

                    if (desc.dynamicSize && originalSizeIt != originalSizes.end()) {
                        auto originalSize = originalSizeIt->second;
                        if (img->width() != desc.width || img->height() != desc.height) {
                            img->resize(desc.width, desc.height);
                        }
                    }
                }
            }
        }
    }

    void RenderGraph::generateImageGraph()
    {
        for (const auto& imageEntry : m_imageResourceDescs) {
            const std::string& imageName = imageEntry.first;
            ResourceGraphVertex v;
            ResourceGraphType::vertex_descriptor vd = boost::add_vertex(v, m_resourceGraph);
            m_resourceVertices[imageName] = vd;
        }
        for (const auto& passEntry : m_passVertex) {
            const std::string& passName = passEntry.first;
            PassGraphType::vertex_descriptor passVd = passEntry.second;
            PassGraphVertex& passData = m_passGraph[passVd];

            std::vector<std::string> outputResources;

            for (uint8_t slot = 0; slot < 8; ++slot) {
                if (!passData.target[slot].empty()) {
                    outputResources.push_back(passData.target[slot]);
                }
            }

            if (!passData.depthStencil.empty()) {
                outputResources.push_back(passData.depthStencil);
            }

            if (outputResources.empty()) {
                continue;
            }

            std::string rootResource = outputResources[0];
            bool foundFixedSizeResource = false;

            for (const auto& resource : outputResources) {
                const ImageResourceDesc& desc = m_imageResourceDescs[resource];
                if (desc.isWnd || (desc.width > 0 && desc.height > 0)) {
                    rootResource = resource;
                    foundFixedSizeResource = true;
                    break;
                }
            }

            for (const auto& resource : outputResources) {
                if (resource != rootResource) {
                    ResourceGraphEdge e;
                    e.type = ResourceGraphEdgeType::ParentChild;
                    boost::add_edge(
                        m_resourceVertices[rootResource],
                        m_resourceVertices[resource],
                        e,
                        m_resourceGraph
                    );
                }
            }

            for (const auto& texture : passData.textures) {
                ImageResourceDesc& textureDesc = m_imageResourceDescs[texture];

                if (textureDesc.width == 0 || textureDesc.height == 0) {
                    ResourceGraphEdge e;
                    e.type = ResourceGraphEdgeType::ParentChild;
                    boost::add_edge(
                        m_resourceVertices[rootResource],
                        m_resourceVertices[texture],
                        e,
                        m_resourceGraph
                    );
                } else {
                    ResourceGraphEdge e;
                    e.type = ResourceGraphEdgeType::Reference;
                    boost::add_edge(
                        m_resourceVertices[texture],
                        m_resourceVertices[rootResource],
                        e,
                        m_resourceGraph
                    );
                }
            }
        }
    }

    void RenderGraph::computeSize()
    {
        constexpr uint32_t DEFAULT_IMAGE_WIDTH = 256;
        constexpr uint32_t DEFAULT_IMAGE_HEIGHT = 256;

        std::set<std::string> processedResources;
        std::queue<std::string> sizeQueue;

        for (const auto& entry : m_windowImageResources) {
            const std::string& imageName = entry.first;
            const WindowImageResource& resource = entry.second;

            m_imageResourceDescs[imageName].width = resource.wnd->getWidth();
            m_imageResourceDescs[imageName].height = resource.wnd->getHeight();

            sizeQueue.push(imageName);
            processedResources.insert(imageName);
        }

        for (const auto& entry : m_windowDepthStencilResources) {
            const std::string& imageName = entry.first;
            const WindowDepthStencilResource& resource = entry.second;


            m_imageResourceDescs[imageName].width = resource.wnd->getWidth();
            m_imageResourceDescs[imageName].height = resource.wnd->getHeight();

            sizeQueue.push(imageName);
            processedResources.insert(imageName);
        }

        for (auto& entry : m_imageResourceDescs) {
            const std::string& imageName = entry.first;
            ImageResourceDesc& desc = entry.second;

            if (!desc.isWnd && desc.width > 0 && desc.height > 0 && processedResources.find(imageName) == processedResources.end()) {
                sizeQueue.push(imageName);
                processedResources.insert(imageName);
            }
        }

        while (!sizeQueue.empty()) {
            std::string currentResource = sizeQueue.front();
            sizeQueue.pop();

            ResourceGraphType::vertex_descriptor currentVertex = m_resourceVertices[currentResource];
            uint32_t currentWidth = m_imageResourceDescs[currentResource].width;
            uint32_t currentHeight = m_imageResourceDescs[currentResource].height;

            ResourceGraphType::out_edge_iterator ei, ei_end;
            std::pair<ResourceGraphType::out_edge_iterator, ResourceGraphType::out_edge_iterator> edgeRange =
                boost::out_edges(currentVertex, m_resourceGraph);
            if (edgeRange.first != edgeRange.second)
            {
                for (boost::tie(ei, ei_end) = boost::out_edges(currentVertex, m_resourceGraph); ei != ei_end; ++ei) {
                    ResourceGraphEdge& edge = m_resourceGraph[*ei];

                    if (edge.type == ResourceGraphEdgeType::ParentChild) {
                        ResourceGraphType::vertex_descriptor childVertex = boost::target(*ei, m_resourceGraph);

                        std::string childResource;
                        for (const auto& entry : m_resourceVertices) {
                            if (entry.second == childVertex) {
                                childResource = entry.first;
                                break;
                            }
                        }

                        if (!childResource.empty() && processedResources.find(childResource) == processedResources.end()) {
                            m_imageResourceDescs[childResource].width = currentWidth;
                            m_imageResourceDescs[childResource].height = currentHeight;

                            processedResources.insert(childResource);
                            sizeQueue.push(childResource);
                        }
                    }
                }
            }
        }

        for (auto& entry : m_imageResourceDescs) {
            const std::string& imageName = entry.first;
            ImageResourceDesc& desc = entry.second;

            if (desc.width == 0 || desc.height == 0) {
                desc.width = DEFAULT_IMAGE_WIDTH;
                desc.height = DEFAULT_IMAGE_HEIGHT;
            }
        }
    }

    void RenderGraph::updateFrameIndices()
    {
        uint32_t currentFrameIndex = m_ctx->currentFrameIndex();
        for (auto* img : m_needUpdataFramesIndexImages) {
            img->changeCurrentIndex(currentFrameIndex);
        }
    }

    void RenderGraph::saveExecutionOrder(const std::vector<PassGraphType::vertex_descriptor>& sortedPasses,
        const std::vector<std::vector<PassGraphType::vertex_descriptor>>& passGroups)
    {
        m_executionOrder.sortedPasses = sortedPasses;
        m_executionOrder.passGroups = passGroups;
    }

    void RenderGraph::generateImageResource()
    {
        computeSize();
        for (const auto& entry : m_windowImageResources) {
            const std::string& imageName = entry.first;
            m_images[imageName] = entry.second.img;
        }

        for (const auto& entry : m_windowDepthStencilResources) {
            const std::string& imageName = entry.first;
            m_images[imageName] = entry.second.img;
        }

        for (const auto& entry : m_imageResourceDescs)
        {
            const std::string& imageName = entry.first;
            const ImageResourceDesc& desc = entry.second;

            if (m_images.find(imageName) != m_images.end()) {
                continue;
            }

            Format format = Format::R8G8B8A8_UNORM;
            if (desc.usage & ImageUsage::DepthStencil) {
                format = Format::D32_SFLOAT_S8_UINT;
            }

            if ((desc.usage & ImageUsage::RenderTarget) || (desc.usage & ImageUsage::DepthStencil)) {
                MutilBufferImage* img = m_ctx->createMutilBufferImage();
                img->imageCount(m_ctx->maxFrameInFlight());
                img->samples(desc.samples);
                img->format(format);
                img->width(desc.width);
                img->height(desc.height);
                img->as(desc.usage);
                img->create();
                m_images[imageName] = img;
                m_needUpdataFramesIndexImages.push_back(img);
                img->changeCurrentIndex(m_ctx->currentFrameIndex());
            } else {
                Image* img = m_ctx->createImage();
                img->samples(desc.samples);
                img->format(format);
                img->width(desc.width);
                img->height(desc.height);
                img->as(desc.usage);
                img->create();
                m_images[imageName] = img;
            }
            ShaderGenerator* gen = m_ctx->getGenerator();

            if (desc.usage & ImageUsage::Texture)
            {
                m_textureLayouts[imageName] = gen->findTextureElementByName(imageName);
                if (m_textureLayouts[imageName]) {

                } else
                {
                    m_textureLayouts[imageName] = TextureElement(imageName.c_str());
                    //todo: 暂时只支持使用现成的textureLayout，走这个分支是个未定义 行为
                    //todo: 完善layout/着色器生成系统系统，以可以在这里动态添加 layout
                }
            }
        }

        for (auto& passEntry : m_passVertex) {
            PassGraphType::vertex_descriptor passVd = passEntry.second;
            PassGraphVertex& passData = m_passGraph[passVd];

            for (uint8_t slot = 0; slot < 8; ++slot) {
                if (!passData.target[slot].empty()) {
                    const std::string& targetName = passData.target[slot];
                    if (m_images.find(targetName) != m_images.end()) {
                        passData.rhiPass->bindTarget(slot, m_images[targetName]);
                    }
                }
            }

            if (!passData.depthStencil.empty()) {
                const std::string& dsName = passData.depthStencil;
                if (m_images.find(dsName) != m_images.end()) {
                    passData.rhiPass->depthStencil(m_images[dsName]);
                }
            }
        }
    }

    void RenderGraph::compile()
    {
        analyzeImplicitDependencies();
        std::vector<PassGraphType::vertex_descriptor> sortedPasses;
        std::unordered_map<PassGraphType::vertex_descriptor, boost::default_color_type> colorMap;

        try {
            boost::topological_sort(m_passGraph, std::back_inserter(sortedPasses),
                                    boost::color_map(boost::make_assoc_property_map(colorMap)));
            std::reverse(sortedPasses.begin(), sortedPasses.end());
        } catch (const boost::not_a_dag& e) {
            ferr << "拓扑排序失败，检查是否存在环" << std::endl;
            return;
        }

        for (auto& passEntry : m_passVertex) {
            PassGraphType::vertex_descriptor passVd = passEntry.second;
            PassGraphVertex& passData = m_passGraph[passVd];

            if (!passData.rhiPass)
            {
                passData.rhiPass = m_ctx->createPass();
                PassClearValue clearValue = passData.pass->getClearValue();
                passData.rhiPass->enableClear(clearValue);
                passData.pass->setRhiPass(passData.rhiPass);
            }
        }

        auto getRenderTargetSignature = [this](PassGraphType::vertex_descriptor passVd) -> std::string {
            PassGraphVertex& passData = m_passGraph[passVd];
            std::stringstream ss;

            for (uint8_t slot = 0; slot < 8; ++slot) {
                ss << "T" << static_cast<int>(slot) << ":" << passData.target[slot] << ";";
            }

            ss << "DS:" << passData.depthStencil;

            return ss.str();
        };

        std::vector<std::vector<PassGraphType::vertex_descriptor>> passGroups;
        std::unordered_set<PassGraphType::vertex_descriptor> assignedPasses;

        for (auto passVd : sortedPasses) {
            if (assignedPasses.find(passVd) != assignedPasses.end()) {
                continue;
            }

            std::string currentSignature = getRenderTargetSignature(passVd);

            std::vector<PassGraphType::vertex_descriptor> currentGroup;
            currentGroup.push_back(passVd);
            assignedPasses.insert(passVd);

            PassGraphType::vertex_descriptor currentPassVd = passVd;
            bool continueChain = true;

            while (continueChain) {
                continueChain = false;

                std::vector<PassGraphType::vertex_descriptor> candidatePasses;

                PassGraphType::out_edge_iterator ei, ei_end;
                for (boost::tie(ei, ei_end) = boost::out_edges(currentPassVd, m_passGraph); ei != ei_end; ++ei) {
                    PassGraphType::vertex_descriptor targetVd = boost::target(*ei, m_passGraph);

                    if (assignedPasses.find(targetVd) != assignedPasses.end()) {
                        continue;
                    }

                    std::string targetSignature = getRenderTargetSignature(targetVd);
                    if (targetSignature == currentSignature) {
                        bool allDependenciesProcessed = true;
                        PassGraphType::in_edge_iterator in_ei, in_ei_end;
                        for (boost::tie(in_ei, in_ei_end) = boost::in_edges(targetVd, m_passGraph); in_ei != in_ei_end; ++in_ei) {
                            PassGraphType::vertex_descriptor sourceVd = boost::source(*in_ei, m_passGraph);
                            if (assignedPasses.find(sourceVd) == assignedPasses.end()) {
                                allDependenciesProcessed = false;
                                break;
                            }
                        }

                        if (allDependenciesProcessed) {
                            candidatePasses.push_back(targetVd);
                        }
                    }
                }

                if (!candidatePasses.empty()) {
                    for (auto targetVd : candidatePasses) {
                        currentGroup.push_back(targetVd);
                        assignedPasses.insert(targetVd);
                    }

                    currentPassVd = candidatePasses.back();
                    continueChain = true;
                }
            }

            passGroups.push_back(currentGroup);
        }

        saveExecutionOrder(sortedPasses, passGroups);

        m_passGroups.clear();

        for (const auto& group : passGroups) {
            RHI::PassGroup* passGroup = m_ctx->createPassGroup();
            m_passGroups.push_back(passGroup);

            for (auto passVd : group) {
                PassGraphVertex& passData = m_passGraph[passVd];
                if (passData.rhiPass) {
                    passGroup->addPass(passData.rhiPass);
                }
            }
        }

        generateImageGraph();
        generateImageResource();

        for (auto& group : m_passGroups) {
            group->create();
        }
    }

    void RenderGraph::analyzeImplicitDependencies()
    {
        std::unordered_map<std::string, std::vector<std::string>> resourceWriters;
        std::unordered_map<std::string, std::vector<std::string>> resourceReaders;

        for (const auto& passEntry : m_passVertex) {
            const std::string& passName = passEntry.first;

            for (const auto& imageEntry : m_imageResourceDescs) {
                const std::string& imageName = imageEntry.first;

                if (isPassWritingToResource(passName, imageName)) {
                    resourceWriters[imageName].push_back(passName);
                }

                if (isPassReadingFromResource(passName, imageName)) {
                    resourceReaders[imageName].push_back(passName);
                }
            }
        }

        for (const auto& resourceEntry : resourceReaders) {
            const std::string& resourceName = resourceEntry.first;
            const std::vector<std::string>& readers = resourceEntry.second;

            if (resourceWriters.find(resourceName) != resourceWriters.end()) {
                const std::vector<std::string>& writers = resourceWriters[resourceName];

                for (const std::string& reader : readers) {
                    for (const std::string& writer : writers) {
                        if (reader != writer) {
                            addPassDenpendency(writer, reader);
                        }
                    }
                }
            }
        }
    }

    bool RenderGraph::isPassWritingToResource(const std::string& passName, const std::string& resourceName)
    {
        auto it = m_passVertex.find(passName);
        if (it == m_passVertex.end()) {
            return false;
        }

        PassGraphType::vertex_descriptor passVertex = it->second;
        PassGraphVertex& passData = m_passGraph[passVertex];

        for (uint8_t slot = 0; slot < 8; ++slot) {
            if (passData.target[slot] == resourceName) {
                return true;
            }
        }

        if (passData.depthStencil == resourceName) {
            return true;
        }

        return false;
    }

    bool RenderGraph::isPassReadingFromResource(const std::string& passName, const std::string& resourceName)
    {
        auto it = m_passVertex.find(passName);
        if (it == m_passVertex.end()) {
            return false;
        }

        PassGraphType::vertex_descriptor passVertex = it->second;
        PassGraphVertex& passData = m_passGraph[passVertex];

        for (const auto& texture : passData.textures) {
            if (texture == resourceName) {
                return true;
            }
        }

        return false;
    }

    void RenderGraph::execute(RHI::CommandBuffer* cmdBuf)
    {
        for (size_t i = 0; i < m_executionOrder.passGroups.size(); ++i) {
            if (i >= m_passGroups.size()) {
                ferr << "PassGroup 索引超出范围: " << i << std::endl;
                continue;
            }

            RHI::PassGroup* passGroup = m_passGroups[i];
            const auto& group = m_executionOrder.passGroups[i];

            passGroup->beginSubmit(cmdBuf);

            for (auto passVd : group) {
                PassGraphVertex& passData = m_passGraph[passVd];
                passData.pass->submit(cmdBuf);
            }

            passGroup->endSubmit(cmdBuf);
        }
    }

    void RenderGraph::swapJobQueue()
    {
        for (auto& passEntry : m_passVertex)
        {
            PassGraphType::vertex_descriptor passVd = passEntry.second;
            PassGraphVertex& passData = m_passGraph[passVd];
            passData.pass->swapJobQueue();
        }
    }
}
