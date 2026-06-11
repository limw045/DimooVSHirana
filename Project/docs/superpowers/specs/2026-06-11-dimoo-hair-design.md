# Dimoo 头发重构设计规格书 (Dimoo Hair Reconstruction Design Spec)

本文档记录了关于优化 Dimoo 3D 模型头发的重构设计方案，旨在将现有的扁平花瓣状发丝，重构为具有高档手办粘土感、流线卷曲形态以及粉紫 Ombré 渐变的精致发丝。

---

## 1. 现状与痛点 (Current Issues)

1. **扁平硬直**：现有的 `drawPetal3D` 发丝几何体为单向渐尖 Ellipsoid，在发根处粗平，发梢处生硬收缩，没有缎带的流线感和弧度，看起来像松果鳞片或花瓣。
2. **色彩杂乱**：现有的 30 个发团是交替着上 5 种不同的马卡龙色（粉、紫、灰绿、薄荷绿、白），像是由不同颜色的补丁拼凑而成，杂乱且缺乏整体感。
3. **塑料反光**：材质的光泽度（Shininess = 84.0f）和镜面反射（Specular = 0.45f）过高，在强光照下反光生硬且刺眼，缺乏手办特有的温润哑光粘土（Matte Clay）质感。

---

## 2. 详细设计方案 (Detailed Design)

### 2.1 几何重构：双端收尖卷曲缎带 (Double-Tapered Curved Ribbon)

修改 `Project/src/DimooModel.cpp` 中的三维顶点生成逻辑：
- **双向收尖**：
  在生成发丝时，将 Taper 因子修改为沿局部长度方向（Y轴）的 Sine 函数或二次曲线：
  $$\text{taper} = \sin\left(\frac{y_u + 1.0}{2} \cdot \pi\right)$$
  其中 $y_u \in [-1, 1]$ 是球体单位坐标。这会使发丝在发根 ($y=-1$) 和发梢 ($y=1$) 的粗度趋近于零，而在中部最饱满。
- **流线卷曲 (Bending)**：
  在顶点绘制前，沿着发丝的主方向（Y轴）引入正弦或二次曲线偏移，为其赋予卷曲弧度：
  $$x_{\text{bent}} = x \cdot \text{taper} + \text{bendX} \cdot (1.0 - y_u^2)$$
  $$z_{\text{bent}} = z \cdot \text{taper} + \text{bendZ} \cdot (1.0 - y_u^2)$$
  通过为不同的发团传入不同的弯曲参数（如 `bendX`, `bendZ`），可以让每股发丝自然贴合头部的弧度。

### 2.2 配色重构：粉紫渐变 (Pink-to-Lavender Ombré)

废除顺序模 5 的纯色交错（`i % 5`）：
- **基准色混合**：
  根据发团在头部模型中的横向或深度坐标（如 $X$ 坐标），将基准色软融合为淡粉红（Pearl Pink）与淡紫（Lavender）。例如偏左侧的发丝更偏紫，偏右侧的发丝更偏粉，或者前额偏粉，脑后偏紫，形成丰富的层次感。
- **发梢渐变 (Tip Ombré)**：
  每个发丝顶部的顶点，其颜色平滑地插值过渡到珍珠白（Pearl White），模拟手办的精细喷笔上色效果。

### 2.3 材质重构：哑光粘土 (Matte Clay Shading)

调整 `applyPetalMaterial` 的光照配置参数，以实现半哑光效果：
- **镜面光衰减**：`specular` 强度从 `0.45f` 调低至 `0.18f`。
- **高光散射**：`shininess` 从较高的 `84.0f` 降低到 `22.0f` 左右，使高光变得更加柔和、扩散，不刺眼。
- **环境光增强**：`ambient` 环境光系数微调高，使暗部和背光面依然有柔和的漫反射色彩，呈现温暖粘土感。

---

## 3. 拟修改的文件 (Proposed File Changes)

- **[DimooModel.cpp](file:///f:/Degree/Last%20Sem/TCG/Project/src/DimooModel.cpp)**：
  - 修改 `drawPetal3D` 几何生成算法，支持双向收尖和三维流线卷曲。
  - 重写 `drawHairClusters` 中的配色插值，从 patchwork 纯色变为粉紫-白渐变。
  - 调整 `applyPetalMaterial` 材质参数为哑光粘土。

---

## 4. 验证方案 (Verification Plan)

1. **编译与运行**：
   在 `Project` 目录下运行 `make` 或编译指令，启动 `DimooViewer` 以三维直观形式验证头发的外观：
   - 检查发丝两端是否平滑收尖，是否有优雅弯曲。
   - 检查色彩是否呈现柔和的粉紫到珍珠白的渐变，且光泽温润。
2. **场景检查**：
   运行游戏场景，确保主角 Dimoo 在游戏界面（Arena）中动作无误，且头发材质与暗盒背景、灯光光束融合完美。
