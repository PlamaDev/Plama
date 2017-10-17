---
layout: "post"
title: "散度，旋度和拉普拉斯算子"
date: "2016-09-30 00:21"
author: Author
catalog: true
tags:
  - 电学
  - 数学
---

> 最后编辑 16年10月24日

这篇的内容可能是会让你们有些意外的，你可能会想 “WOC 这个码农什么时候研究起场论来了？”。这个其实是我的专业课内容，而我的专业课学的又十分吃力，所以我就把我能听懂的写出来，忘了的时候就来查一查，仅此而已。如有谬误，还烦请各位指正。

## 散度

#### 微分解释

因为是个电气狗，所以啥都是从电学说起。我看知乎上好多大神都是在说流体力学，不过不好意思我不会。说之前先说这个：

$$\mathbf{D} = \epsilon\mathbf{E}$$

这个式子里 $D$ 就是所谓的电通密度，$ε$ 是介电常数。式子本身只是一个换算关系，重点在于他把电场强度这个介质有关的量转换成了介质无关的电通密度，相似的还有另一个 $\mathbf{B} = \mu\mathbf{H}$。然后我们把麦克斯韦方程组里的高斯定理条拿出来：

$$\oiint _S\textbf{D}\cdot d\textbf{S} = q_0$$

电通密度在闭合曲面上的面积积分（为简洁起见，下文记为 $\Phi D$）等于包含的电荷量。这作为电学一个非常基础的结论，也就是今天的内容的起点。这是一个积分结论，如果我们需要微分的表达方式呢？在这个三维空间内，我们第一能想到的微分对象就是体积了。于是我们对体积微分：

$$
\begin{align}
\lim_{V\to 0}\frac{\mathbf{D}\cdot d\mathbf{S}}{V} &= \lim_{V\to 0}\frac{q_0}{V} \tag{1a}\\
\lim_{V\to 0}\frac{\mathbf{D}\cdot d\mathbf{S}}{V} &= \rho_0\label{1}\tag{1b}
\end{align}
$$

到这一步先别急，你可能还记得高斯定理的微分式：

$$\nabla\cdot\mathbf{D} = \rho_0\tag{2}\label{2}$$

这时候你对比一下 $(\ref{1})$ 和 $(\ref{2})$，等式右边其实是一致的，所以结论也显而易见：

$$\nabla\cdot\mathbf{D} = \lim_{V\to 0}\frac{\mathbf{D}\cdot d\mathbf{S}}{V}$$

散度就是一个矢量场在一定体积内面积分的体微分。很重要的一点是：散度是一个微分结论，所以它最重要的场合是 “在这个点上的散度是多少”，而不是 “这个场的散度是多少”。当然你如果非要这么说也没法说不对就是了，毕竟速度是一个微分量，但是也分瞬时速度和平均速度对吧。

#### 宏观解释

作为一个微分量。他的宏观意义其实并不像微观意义那么重要，但是在知乎上这一部分是比较着重被提到的（或者很容易被这样误解），因为这一部分比较好理解吧。它在符号上与 $\Phi D$ 一致，数值上等于 $\Phi D / V$, 如果你对 $\Phi D$ 有不错的了解，这一部分其实并没有什么难度。你可以把它理解成场源的密度（见 $(\ref{2})$），但是这样做意义不大，因为不是所有的微分量都能有具体的物理意义。

#### 坐标解释

当然我这里只说直角坐标系，柱坐标和球坐标参见 wiki。你可以看到这样一个公式：如果向量场 $\mathbf{A}$ 可以表示成以下形式：

$$\mathbf{A}(x,y,z) = P(x,y,z)\mathbf{i} + Q(x,y,z)\mathbf{j} + R(x,y,z)\mathbf{k}$$

那么散度表示为：

$$\nabla\cdot\mathbf{A} = \frac{\partial P}{\partial x} + \frac{\partial Q}{\partial y} + \frac{\partial R}{\partial z}$$

下面我通过空间内点电荷所产生的电场为例，来求这个电场的散度。假设点电荷置于原点，空间内点电荷所产生的电场可以表示为：

$$\mathbf{E} = \frac{kQ}{r^2}\cdot\mathbf{i}$$

其中 r 表示原点到任意一点 P 的距离，$\mathbf{i}$ 表示 $\vec{OP}$ 方向的单位向量，转化为坐标表示：

$$
\begin{split}
\mathbf{E} &= \frac{kQ}{x^2+y^2+z^2}\cdot\frac{\vec{OP}}{|\vec{OP}|} \\ &= \frac{kQ}{(x^2+y^2+z^2)^{3/2}}\cdot(x,y,z) \\
&= (\frac{xkQ}{(x^2+y^2+z^2)^{3/2}}, \frac{ykQ}{(x^2+y^2+z^2)^{3/2}},\frac{zkQ}{(x^2+y^2+z^2)^{3/2}})
\end{split}
$$

于是：

$$\begin{split}
\nabla\cdot\mathbf{E}  = & \frac{\partial P}{\partial x} + \frac{\partial Q}{\partial y} + \frac{\partial R}{\partial z} \\
=& \frac{kQ(x^2+y^2+z^2)^{3/2}-xkQ\cdot\frac{3}{2}\sqrt{x^2+y^2+z^2}\cdot 2x}{(x^2+y^2+z^2)^3} + \\ &\frac{kQ(x^2+y^2+z^2)^{3/2}-ykQ\cdot\frac{3}{2}\sqrt{x^2+y^2+z^2}\cdot 2y}{(x^2+y^2+z^2)^3} + \\ &\frac{kQ(x^2+y^2+z^2)^{3/2}-zkQ\cdot\frac{3}{2}\sqrt{x^2+y^2+z^2}\cdot 2z}{(x^2+y^2+z^2)^3} \\
=& \frac{0}{(x^2+y^2+z^2)^3}
\end{split}$$

当 x，y，z 不同时等于0时，$\nabla\cdot\mathbf{E} = 0$，当 x，y，z 全等于零时，这个式子并不足于描述这个情况。如果用微分推的话，原点的散度应该等于正无穷。这个可能和大家对日常概念的感知有所出入，但是我们换个角度来说，既然我们之前已经说了，电场的散度等于电荷密度，而这里用的又是点电荷，也就意味着在这一点电荷的密度是无穷大，而除此之外的点密度都是零，这就和我们的理解一致了。

## 旋度

#### 微分解释

旋度其实和散度的定义是非常相似的。散度是矢量场的面积分（通量）的体微分，旋度作为一个向量，它与面的法向量的点积则是矢量场的环路积分（环量）的面微分。麦克斯韦方程组中安培定律的积分式这样写道：

$$\oint_L\mathbf{B} d\mathbf{l} = \mu_0 I + \mu_0\epsilon_0\frac{d\Phi_\mathbf{E}}{dt}$$

我们对他两侧取面积的微分：

$$\lim_{S\to 0}\frac{\oint_L\mathbf{B} d\mathbf{l}}{S} = \lim_{S\to 0}(\mu_0\frac{I}{S} + \mu_0\epsilon_0\frac{d\Phi_\mathbf{E}}{Sdt})\tag{3}\label{3}$$

根据定义，我们有：

$$\mathbf{J} = \frac{I}{S} \cdot \mathbf{n}\tag{4}\label{4}$$
$$\Phi_\mathbf{E} = \int_S \mathbf{E}\cdot d\mathbf{A}\tag{5}\label{5}$$

其中 $S$ 为面积，$\mathbf{n}$ 为法向量。在极限情况下，$(\ref{5})$ 可以化简成下式：

$$\Phi_\mathbf{E} = S(\mathbf{E} \cdot \mathbf{n})\tag{6}\label{6}$$

将 $(\ref{4})$ 和 $(\ref{6})$ 代入 $(\ref{3})$，可得：

$$\lim_{S\to 0}\frac{\oint_L\mathbf{B} d\mathbf{l}}{S} = \mu_0\mathbf{J}\cdot\mathbf{n} + \mu_0\epsilon_0\frac{\partial\mathbf{E}\cdot\mathbf{n}}{\partial t}$$

安培定则的微分表达式如下：

$$\nabla\times\mathbf{B} = \mu_0\mathbf{J} + \mu_0\epsilon_0\frac{\partial\mathbf{E}}{\partial t}$$

通过对比，我们可以清楚的看出这个结论：

$$\nabla\times\mathbf{B} \cdot \mathbf{n} = \lim_{S\to 0}\frac{\oint_L\mathbf{B} d\mathbf{l}}{S}$$

旋度区别于散度的一点就是它是一个向量。如你所见，我们之前的推导是基于一个任意的面（和包围它的环路）进行的，也就是说，只要是某一点附近的任何一个面，散度的结论都对它有效。所以很重要的一点是，旋度和散度一样，是一个点的微分特性，不依赖于任何一个特定的面。

#### 宏观解释

这个地方我只能说，旋度几乎不能从宏观角度来看。举个例子：如果我在空间里随便画一个环路，甚至不必要使他全部在一个平面内，那么他围成的面的法向量是什么呢？这个问题会搞得很复杂而且没有意义。我们之前的推导有一些步骤本来就只能在微分的环境下进行，所以它注定也只能是一个场之中点的性质。

我觉得我之前长期被一个思想误导了：“散度和旋度是描述场的两个尺度”，实际上你并不能说 “这个场的散度是多少，旋度是多少”，因为散度和旋度本身就是点的特性，而要想描述一个场，则要通过散度函数和旋度函数来描述这个场里任一点的散度和旋度。我觉得这一点很重要，对于一个场，散度和旋度并不是一个数值，而是一个函数。

## 其他的东西

#### 梯度，散度和旋度的横向比较

这一段里边出现的东西可能是相当抽象的，但是我打赌你会受惊于数学之美。我们学习导数的时候，会有这样一个东西 $\partial / \partial x$, 用他乘上 F，就得到了 F 的导数。相应的，我们在研究向量场的时候也给出这样一个量，姑且叫他向量 $\nabla$ 好了：

$$\nabla = (\frac{\partial}{\partial x}, \frac{\partial}{\partial y}, \frac{\partial}{\partial z})$$

对于标量场 F，我们有：

$$\text{梯度} = \nabla F$$

对于矢量场 $\mathbf{F} = (f_x, f_y, f_z)$，我们有：

$$\text{散度} = \nabla\cdot\mathbf{F}$$

$$\text{旋度} = \nabla\times\mathbf{F}$$

不用谢我，数学就是这么简单（放屁）。


#### 拉普拉斯算子

如果你对于我之前说的东西（当然还包括没说的梯度）了然于心的话，这个其实没啥可说的：

$$\Delta f = \nabla^2 f = \nabla \cdot \nabla f$$

拉普拉斯算子定义为梯度的散度。式中的 $f$ 是一个标量场，经过运算之后同样得到一个标量场。举一个最常见的例子，电势经过拉普拉斯算子运算后得到电荷密度。当然拉普拉斯算子在很广的领域有其他的作用，也有一些特性，不过这篇文章仅供理解，所以就不多说了。

## 附言

这一篇内容我其实一直是在通过结论推过程。毕竟这种算子的存在只是来自于定义，光说定义可能并不好理解。所以不如从大家比较熟悉的两端出发，逐渐合拢，这样可能更易懂些。内容我还没写完，缓更。另外我其实预料到这些内容可能我们很快就会重新学到，不过这也无所谓啦，正好当作学习笔记了（笑）
