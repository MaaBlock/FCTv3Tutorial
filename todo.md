# todo:
```mermaid
graph TD
    1[RenderGraph添加资源的时候可以指定image的format， 并且不影响其图像大小的推导] 
    2[优化矢量渲染的命令队列大小]
    3[RenderGraph支持分析并行，和同步]
    4[同步量的管理]
    5[优化矢量渲染里的上传，同步从cpu fence同步改为gpu信号量同步]
    
    5 --> 4 --> 3
    2 --> 1
    
    6[CEGUI Intergration]
    7[CEGUIRenderer]
    8[CEGUITexture]
    9[CEGUITextureTarget]
    10[CEGUIRenderTarget]
    11[CEGUIWindowTarget]
    12[CEGUIGeometryBuffer]
    
    6 --> 7
    6 --> 8
    6 --> 9
    6 --> 10
    6 --> 11
    6 --> 12
    9 --> 8
    9 --> 10
    11 --> 10
    
    13[image回读到cpu]
    14[实现renderer的scissorEnabled,
        启用时每次渲染把viewport设置为正常的，不启用时设置为scissor]
    15[autoReviewport对于scissorEnabled作支持]
    8 --> 13
    14 --> 15
    7 --> 14
    
    16[nodejs intergration带来的构建安装问题]
    17[nasm自动安装]
    18[编写nodejs专用 cmakelist]
    19[执行构建命令]
    20[添加安卓的构建]
    21[构建后的文件添加到目标中]
    16 --> 17    
    16 --> 18
    18 --> 17
    18 --> 19
    18 --> 20
    18 --> 21
