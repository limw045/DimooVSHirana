# 盲盒格斗游戏架构设计与详细开发计划 (Architecture & Development Plan)

本开发计划书由项目资深格斗游戏开发者与软件架构师联合制定。本计划书将作为本项目（Hirono vs Dimoo 格斗游戏）后续全部代码实现、系统集成与测试验证的唯一技术规范标准。

---

## 一、 系统架构总览 (System Architecture Overview)

由于本项目限制使用 Code::Blocks 并在 MinGW 编译器、标准 GLUT/OpenGL 1.1/2.0 固定管线（Fixed-Function Pipeline）环境下运行，架构设计必须在保证性能的前提下，避免复杂的现代 GPU Shader 依赖，完全使用 CPU 端逻辑控制与固定着色管线配合来实现复杂的视觉效果。

游戏系统设计为经典的 **逻辑与渲染分离架构（Logical & Visual Decoupling）**：

```
                              +--------------------------+
                              |        Win32 API         |
                              |   GetAsyncKeyState       |
                              +------------+-------------+
                                           | Polling Key States
                                           v
+-----------------------+     +------------+-------------+
|     OpenGL / GLUT     |     |   Input Manager Module   |
|   display callback    |     | (Input Buffer & KeyState)|
+-----------+-----------+     +------------+-------------+
            ^                              |
            | Read Transforms              | Update Inputs
            |                              v
+-----------+-----------+     +------------+-------------+
|    Render Pipeline    |     |  Fixed-Timestep GameLoop |
| (Opaque & Translucent)|<----+  (glutTimerFunc, dt=16ms)|
+-----------------------+     +------------+-------------+
                                           |
                                           v
                              +------------+-------------+
                              |   Physics & Collisions   |
                              |   (2.5D Movement & AABB) |
                              +------------+-------------+
                                           |
                                           v
                              +------------+-------------+
                              |      State Machine       |
                              | (Character & Prop States)|
                              +--------------------------+
```

---

## 二、 核心技术模块详述 (Module Specifications)

### 1. 输入处理模块 (Input & Combo System)
*   **技术痛点**：标准 GLUT 的键盘释放事件回调（`glutKeyboardUpFunc` / `glutSpecialUpFunc`）在 MinGW 环境中常因动态链接库（`glut32.dll`）兼容性问题导致运行时入口缺失错误。如果只使用按下事件，角色将无法正常判定“停止移动”。
*   **解决方案**：使用 Windows 原生 API `<windows.h>` 中的 `GetAsyncKeyState` 进行轮询，或利用带定时器心跳包的 GLUT 输入过滤算法。
*   **架构设计**：
    *   `InputManager` 类维护一个包含 256 个按键状态的布尔数组 `bool m_keyStates[256]`。
    *   在每帧逻辑更新（`glutTimerFunc`）的开头，轮询指定按键的状态（A、D、W、J、K、L等）。
    *   **搓招队列 (Input Buffer)**：维护一个环形输入队列 `std::deque<InputEvent>`。每个输入事件记录按键类型与时间戳。当检测到特定时序按键组合（例如：`↓` `→` `Attack`）时，触发特定招式状态转换。

### 2. 物理与碰撞引擎 (Physics & Collision Engine)
为了确保格斗游戏判定的精确与硬核感，采用 **2.5D 降维轨道判定物理**：
*   **空间坐标映射**：
    *   水平轴：$X \in [-3.0, 3.0]$。
    *   垂直轴（跳跃/重力）：$Y \ge 0.0$。
    *   景深轴：固定在 $Z = 0.0$，仅在渲染特效和模型宽度时存在。
*   **重力与位移**：
    *   使用 Euler 积分计算位移：
        $$Y_{t} = Y_{t-1} + V_{y, t-1} \cdot dt$$
        $$V_{y, t} = V_{y, t-1} - g \cdot dt \quad (\text{if } Y_t > 0 \text{, else } Y_t = 0, V_{y} = 0)$$
    *   其中，重力常数 $g$ 设置为 $18.0 \text{ units/s}^2$（适度加速下坠以增强打击节奏感）。
*   **碰撞盒子（Hitbox & Hurtbox）**：
    *   每个实体拥有两个 2D 矩形包围盒（AABB）：
        *   `Hurtbox (受击盒)`：随帧动画偏移（通常为角色身体中心区域）。
        *   `Hitbox (攻击盒)`：仅在攻击动作的有效判定帧激活，相对于角色原点向攻击方向偏置。
    *   **边界撞击 (Wall Collision Bounce)**：
        *   当角色 $X$ 坐标越过 $\pm 3.0$ 时，触发边界撞击。若此时角色处于受击飞起状态（`HIT_FLUNG`），则以 $V_x \leftarrow -0.5 \cdot V_x$ 的反弹速度弹回，同时触发盒壁震动物理。

### 3. 三维层次建模与图形缓存 (Hierarchical Modeling & Caching)
由于 OpenGL 1.x / 2.0 固定管线在绘制复杂几何体时，CPU-GPU 数据传输是核心瓶颈，所有不发生形变的基础几何体必须编译为 **显示列表 (Display Lists)**：
*   **小野 (Hirono) 建模细节**：
    *   `DisplayList_HironoHair`：包含 11 个以不同球坐标偏置、旋转的 `glutSolidSphere` 组合，颜色使用漫反射橙黄色。
    *   `DisplayList_RoseDomeBase`：包含玫瑰罩的圆形木质底座，表面刻画 B-612 字符。
    *   运动结构：躯干（圆柱）作为主坐标系，左右臂及围巾段作为局部子坐标系，使用 `glPushMatrix` / `glPopMatrix` 串联。
*   **Dimoo 建模细节**：
    *   `DisplayList_VineRing`：预先计算 24 节点 Bezier 样条曲线上每一点的切线方向，并在初始化时用细小圆锥面片将其连接为深棕色藤蔓环，随节点相位参数作局部随机扰动。
    *   运动结构：盘坐姿态整体平移，蝴蝶翅膀绕中轴 Y 作正弦振荡（$angle = \sin(t \cdot \omega) \cdot 35^\circ$）。

### 4. 双通道混合渲染管线与透明度排序 (Rendering Pipeline)
为解决透明材质造成的“深度剔除”错误，设计以下双通道渲染管线：

#### 步骤一：不透明通道 (Opaque Pass)
1. 启用深度写入 `glDepthMask(GL_TRUE)`，启用深度测试 `glEnable(GL_DEPTH_TEST)`。
2. 绘制纸盒底面、盒壁内侧印刷图案、干燥剂、说明书、配件小袋。
3. 绘制小野身体、头发、围巾、底座。
4. 绘制 Dimoo 藤蔓环干、头顶停落蝴蝶。

#### 步骤二：半透明通道 (Translucent Pass)
1. 关闭深度写入 `glDepthMask(GL_FALSE)`，开启混合 `glEnable(GL_BLEND)` 并设置混合函数：
   `glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)`。
2. **深度距离排序 (Depth Sorting)**：
   计算每个半透明物体相对于相机的位置向量 $\mathbf{D}_{entity} = \mathbf{P}_{entity} - \mathbf{P}_{camera}$，根据其模长进行降序排序。
3. 按照“从远到近”的顺序依次绘制：
   *   大招地面白雾层。
   *   小野的半透明红色披风。
   *   Dimoo 珍珠白粉半透明身体（$\alpha = 0.88$）。
   *   Dimoo 藤蔓庇护护盾薄膜（半透明绿色）。
   *   小野玫瑰罩玻璃体（透明蓝色，$\alpha = 0.35$）。
   *   所有飘散的半透明粒子系统（花瓣、纸屑、光环线框）。
4. 恢复 `glDepthMask(GL_TRUE)`，关闭混合。

#### 步骤三：屏幕正交投影 UI 通道 (Ortho UI Pass)
1. 切换投影模式至正交投影 `glOrtho`，重置模型视图矩阵。
2. 绘制融合于盒壁的纸质标签血条（利用盒壁在屏幕空间的映射位置）。
3. 绘制技能冷却指示器、伤害飘字（LED 样式线段）。

---

## 三、 阶段性里程碑与交付计划 (Milestones)

### 阶段 1：基础核心骨架搭建 (Phase 1)
*   **开发内容**：
    1.  建立 Code::Blocks 干净的项目目录结构，配置好标准 GLUT 的 MinGW 编译环境。
    2.  实现基于 `<windows.h>` 的高精度计时器与基于 `glutTimerFunc` 的固定步长游戏循环（`dt = 16.67ms`）。
    3.  实现 `InputManager` 输入管理器底座。
*   **自检验证**：编译通过无错，运行日志输出稳定，能够通过 WASD 键平滑（无物理抖动、无释放延迟）控制一个占位长方体左右移动与跳跃落地的物理动作。

### 阶段 2：角色层次建模与显示列表优化 (Phase 2)
*   **开发内容**：
    1.  在代码中纯手工编写小野的层次建模（包括头部、11发块、围巾拼接、双臂和披风）。
    2.  纯手工编写 Dimoo 盘坐形态及背后 24 节点藤蔓的贝塞尔插值算法。
    3.  使用 `glGenLists` 和 `glNewList` 对不发生形变的静态几何组件进行编译缓存。
*   **自检验证**：在开启 3 盏光源（主灯、小野金色点光、Dimoo 蓝白点光）的前提下，两个角色同屏渲染，帧率稳定在 60 FPS 满帧，几何构造高度贴合手办设计特征。

### 阶段 3：2.5D 物理碰撞、墙壁反弹与状态机 (Phase 3)
*   **开发内容**：
    1.  建立 `CharacterState` 动作状态机（Idle, Walk, Attack, Hit, Special, KO）。
    2.  实现 AABB 碰撞检测算法，包括角色间的水平挤压防穿透判定。
    3.  实现纸盒边界碰撞反弹物理（受击飞后撞击盒壁，盒壁触发衰减简谐震荡 $e^{-\lambda t}\cos(\omega t)$，角色反弹）。
*   **自检验证**：一角色被另一角色击飞后，在撞击纸盒左右壁时发生物理反弹，且纸盒内壁发生晃动，角色无穿墙现象。

### 阶段 4：粒子特效、技能机制与透明管线 (Phase 4)
*   **开发内容**：
    1.  编写统一的粒子系统管理器（支持花瓣粒子、碎玻璃粒子、白粉尘粒子以及白色小蝴蝶粒子）。
    2.  实现小野的花瓣普攻轨迹（正弦飘动）、B-612 光圈、护盾球以及大招星空覆盖正交图层。
    3.  实现 Dimoo 的梦蝶飞击（波形飞行）、入梦同心圆光环、藤蔓护盾膜破损爆炸以及梦境蔓延效果。
    4.  集成双通道混合渲染逻辑。
*   **自检验证**：在任意大招爆发或护盾展开时，透明通道物体混合正确，未发生画面穿帮或深度黑影，特效飞散速度恒定。

### 阶段 5：道具物理响应、UI 融合与整体验证 (Phase 5)
*   **开发内容**：
    1.  实现盒底包装道具的完整物理交互（踩在气泡纸上的形变、产品说明书的受击飞舞、配件袋的破裂零件弹跳滚动）。
    2.  绘制融合于内盒壁的纸标签 HUD（血条卷角）与 3D LED 数字伤害浮空飘字。
    3.  编写完整的自动化集成测试，检查是否有 OpenGL 状态码污染。
*   **自检验证**：完美实现从盲盒盒盖顶开/化蝶登场到对战决胜的完整流程。运行中 `glGetError()` 返回 0。

---

## 四、 答辩与评分点自检清单 (Evaluation Alignment)

1.  **关于代码生成器禁用规避**：本工程所有三维图形的顶点与变换代码将全部采用 C++ 显式手工计算，杜绝任何第三方 Maya/3DS Max 自动转换生成的 OpenGL 代码片，完全符合大纲 Golden Tips 的 BEWARE 安全性条款。
2.  **建模复杂度 (14分)**：小野的 11 发块不规则拼合、小王子服装与围巾、半透明玫瑰罩、月球坑底座，以及 Dimoo 的盘坐悬浮姿态、背后扭曲藤蔓圈与环绕轨道蝴蝶，具备极高的自制几何建模复杂度，能极大拉高“Construction of Characters”评分。
3.  **渲染效果与技能动作 (14分)**：采用的局部点光源照亮、透明混合排序渲染管线、大招星空正交投影覆盖、体积光锥效果以及独立的粒子管理器，能充分满足“Animation of Characters Skills”中对于光照、混合与特效的严苛要求。
4.  **环境响应度 (8分)**：环境损伤系统（盒壁凹陷、弹射碎纸屑、划痕、波点色调随血量震动）、动态可破坏盒盖、可踩碎气泡纸、可吹飞说明书，完美击中“Battle Environment”评分大点。
5.  **工程结构 (4分)**：逻辑与渲染分离，全工程中文技术讲解，代码、注释及日志文件纯英文，注释使用标准的 Google Style 风格定义（Args/Returns/Raises），体现卓越工程素养。
