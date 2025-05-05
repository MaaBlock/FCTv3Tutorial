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
        auto& vertex = m_passGraph[vd];

        auto passResourceCallback = [this, name](PassResource* resource) {
            auto passVdIt = m_passVertex.find(name);
            if (passVdIt == m_passVertex.end()) {
                return;
            }

            PassGraphType::vertex_descriptor passVd = passVdIt->second;
            PassGraphVertex& passData = m_passGraph[passVd];

            for (const auto& textureName : passData.textures) {
                auto textureIt = m_textureLayouts.find(textureName);
                auto imageIt = m_images.find(textureName);

                if (textureIt != m_textureLayouts.end() && imageIt != m_images.end())
                {
                    resource->setTexture(imageIt->second,textureIt->second);
                    resource->update();
                }
            }
        };
        pass->setPassResourceAddCallback(passResourceCallback);
    }

    void RenderGraph::addWindowResource(Window* wnd)
    {
        if (std::find(m_bindedWindows.begin(), m_bindedWindows.end(), wnd) != m_bindedWindows.end())
        {
            return;;
        }
        m_bindedWindows.push_back(wnd);
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

    void RenderGraph::bindTextureImage(std::string name, std::string image, uint32_t width, uint32_t height,
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
        uint32_t currentFrameIndex = m_ctx->currentSubmitFrameIndex();
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

            Format format = Format::R32G32B32A32_SFLOAT;
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
                img->changeCurrentIndex(m_ctx->currentSubmitFrameIndex());
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
                    gen->allocateTextureBinding(m_textureLayouts[imageName]);
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
    void RenderGraph::printExecutionOrder()
    {
        if (m_executionOrder.passGroups.empty()) {
            fout << "渲染图尚未编译，无法显示执行顺序" << std::endl;
            return;
        }

        fout << "渲染图执行顺序:" << std::endl;

        for (size_t i = 0; i < m_executionOrder.passGroups.size(); ++i) {
            const auto& group = m_executionOrder.passGroups[i];

            fout << "Group " << i << ":" << std::endl;

            for (auto passVd : group) {
                PassGraphVertex& passData = m_passGraph[passVd];

                std::string passName = "未知";
                for (const auto& entry : m_passVertex) {
                    if (entry.second == passVd) {
                        passName = entry.first;
                        break;
                    }
                }

                fout << "  - " << passName << std::endl;
            }
        }
    }
    void RenderGraph::printResourceInfo()
    {
        fout << "\n渲染图资源信息:" << std::endl;
        fout << "总资源数量: " << m_imageResourceDescs.size() << std::endl;

        std::vector<std::string> renderTargets;
        std::vector<std::string> depthStencils;
        std::vector<std::string> textures;
        std::vector<std::string> windowResources;

        for (const auto& entry : m_imageResourceDescs) {
            const std::string& imageName = entry.first;
            const ImageResourceDesc& desc = entry.second;

            if (desc.isWnd) {
                windowResources.push_back(imageName);
            } else if (desc.usage & ImageUsage::RenderTarget) {
                renderTargets.push_back(imageName);
            } else if (desc.usage & ImageUsage::DepthStencil) {
                depthStencils.push_back(imageName);
            } else if (desc.usage & ImageUsage::Texture) {
                textures.push_back(imageName);
            }
        }

        if (!windowResources.empty()) {
            fout << "\n窗口资源:" << std::endl;
            for (const auto& name : windowResources) {
                const ImageResourceDesc& desc = m_imageResourceDescs[name];
                fout << "  - " << name << " (" << desc.width << "x" << desc.height << ")";

                Image* img = m_images[name];
                if (img) {
                    fout << " 格式: " << formatToString(img->format());
                }

                fout << std::endl;
            }
        }

        if (!renderTargets.empty()) {
            fout << "\n渲染目标:" << std::endl;
            for (const auto& name : renderTargets) {
                const ImageResourceDesc& desc = m_imageResourceDescs[name];
                fout << "  - " << name << " (" << desc.width << "x" << desc.height << ")";

                auto imgIt = m_images.find(name);
                if (imgIt != m_images.end()) {
                    Image* img = imgIt->second;
                    fout << " 格式: " << formatToString(img->format());

                    MutilBufferImage* multiImg = dynamic_cast<MutilBufferImage*>(img);
                    if (multiImg) {
                        fout << " 多缓冲: 是 (数量: " << multiImg->imageCount() << ")";
                    }
                }

                fout << std::endl;
            }
        }

        if (!depthStencils.empty()) {
            fout << "\n深度模板缓冲:" << std::endl;
            for (const auto& name : depthStencils) {
                const ImageResourceDesc& desc = m_imageResourceDescs[name];
                fout << "  - " << name << " (" << desc.width << "x" << desc.height << ")";

                // 输出格式
                auto imgIt = m_images.find(name);
                if (imgIt != m_images.end()) {
                    Image* img = imgIt->second;
                    fout << " 格式: " << formatToString(img->format());
                }

                fout << std::endl;
            }
        }

        if (!textures.empty()) {
            fout << "\n纹理:" << std::endl;
            for (const auto& name : textures) {
                const ImageResourceDesc& desc = m_imageResourceDescs[name];
                fout << "  - " << name << " (" << desc.width << "x" << desc.height << ")";

                auto imgIt = m_images.find(name);
                if (imgIt != m_images.end()) {
                    Image* img = imgIt->second;
                    fout << " 格式: " << formatToString(img->format());
                }

                auto layoutIt = m_textureLayouts.find(name);
                if (layoutIt != m_textureLayouts.end()) {
                    const TextureElement& element = layoutIt->second;
                }

                fout << std::endl;
            }
        }

        fout << "\n资源依赖关系:" << std::endl;
        for (const auto& entry : m_resourceVertices) {
            const std::string& resourceName = entry.first;
            ResourceGraphType::vertex_descriptor vd = entry.second;

            ResourceGraphType::out_edge_iterator ei, ei_end;
            for (boost::tie(ei, ei_end) = boost::out_edges(vd, m_resourceGraph); ei != ei_end; ++ei) {
                ResourceGraphEdge& edge = m_resourceGraph[*ei];
                ResourceGraphType::vertex_descriptor targetVd = boost::target(*ei, m_resourceGraph);

                std::string targetName = "未知";
                for (const auto& targetEntry : m_resourceVertices) {
                    if (targetEntry.second == targetVd) {
                        targetName = targetEntry.first;
                        break;
                    }
                }

                std::string edgeType = (edge.type == ResourceGraphEdgeType::ParentChild) ? "父子关系" : "引用关系";
                fout << "  " << resourceName << " -> " << targetName << " (" << edgeType << ")" << std::endl;
            }
        }
    }

    std::string RenderGraph::formatToString(Format format)
    {
        switch (format) {
        case Format::R8G8B8A8_UNORM: return "R8G8B8A8_UNORM";
        case Format::R8G8B8A8_SRGB: return "R8G8B8A8_SRGB";
        case Format::R32G32B32A32_SFLOAT: return "R32G32B32A32_SFLOAT";
        case Format::R16G16B16A16_SFLOAT: return "R16G16B16A16_SFLOAT";
        case Format::D32_SFLOAT: return "D32_SFLOAT";
        case Format::D32_SFLOAT_S8_UINT: return "D32_SFLOAT_S8_UINT";
        case Format::D24_UNORM_S8_UINT: return "D24_UNORM_S8_UINT";
        default: return "未知格式";
        }
    }

    void RenderGraph::updateResource()
    {
        for (const auto& passEntry : m_passVertex) {
            PassGraphType::vertex_descriptor passVd = passEntry.second;
            PassGraphVertex& passData = m_passGraph[passVd];
            passData.pass->updateResource();
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
