---
layout: "post"
title: "LaTeX 中的排版细节——中文渲染与图文混排"
date: "2016-08-16"
author: Author
catalog: true
tags:
  - LaTeX
---

> 五月对于每个电气狗来说都是充满了悲伤的，说起来我手上两个礼拜内还有六七个 ddl，对，大部分都是大 ddl。之所以开始动笔写这篇，有两个原因，一是最近写了太多的 report，我也开始对 LaTeX 有一些表层的理解，另外是现在在上工程数学，这门课不找点别的事情做实在是太过于无聊，所以我只好趁机更新一下。这篇文章会记录我目前对于 LaTeX 的一些理解，当然仍然是基于 atom 的编辑环境。实话说我在 LaTeX 上完全就是半吊子水平，把这些东西写出来主要是为了分享与记录，所以不要对我这些内容的准确性有过多的期待。如果能有所帮助，自然是最好的。

我现在每篇文章基本拿到手都要重新写前言。如你所见，这篇文章其实我很早就开始动笔了。近来有空就把它们全部整理一下发出来。

## 中文输入

我目前的解决方案是 XeTeX，这是一个新的 TeX 引擎，最大的优势就在于他内建了对于东亚文字的兼容。 如果你是在 atom 环境下，我的建议是用 “LaTeX” 这个包，下边就可以选择渲染引擎了。这个包相比 “LaTeX-plus” 另外一个特性是他在 windows 环境下不会有路径错误的问题，用起来很方便。我目前使用的 LaTeX 发行版，来自于 tex live 最新版，配置简单，工作正常。有人说原生 XeTeX 就可以写中文，我这边似乎并不能直接用，需要导入 xeCJK 这个包才能正常写中文（似乎是因为原生字体并不能显示中文？）。至于 MikTex 能不能用我也并不清楚，似乎要装 CTeX 这个包，安装似乎并不是很方便，所以我就没多查。

如果要用 CTeX 的话，我个人的建议是直接下载一个完整的 CTeX 环境，不过那个跟 atom 没啥关系，这个环境自带了 WinEdit 作为编辑器，也是国内大部分新人上手的选择。但是这个环境我个人并不是很推荐，一方面是由于一些版权的问题，另外 CTeX 更改了原版 LaTeX 的一部分逻辑，对新人可能会有不好的影响。实际上 CTeX 是基于 MikTeX 的，更像是对原先引擎的补丁，而 XeTeX 是通过新的架构解决了文字的显示问题。所以XeTeX更有可能是将来的大趋势。

## 西文字体

#### 内建字体包

实话说 LaTeX 自带的字体我并不喜欢，虽说识别度很高，但是总觉得有点扭扭捏捏的。如果你只使用英文的话，可以这样写

```LaTeX
\usepackage{times}
```

 这样就可以将文本字体替换成 Times New Roman, 除此之外，还有其他一些字体包可用，谷歌一下应该就有了。这里我插张图对比一下原生字体与 Times New Roman 的效果。相比而言 Times 的字体要更重，而且字形也更硬朗。

![字体对比][1]

#### 自定义字体

当然这可能还是不能满足我们的需要。如果要用自定义的字体的话，就需要这样写:

```LaTeX
\usepackage{fontspec, xunicode, xltxtra}    % 字体设置包
\setmainfont{Times New Roman}    % 设置正文字体
```

就可以使用你机器上安装了的所有字体了，so easy。值得注意的是，自定义的字体并不支持 LaTeX 的一些文字排版特性，比如fi的自动连字。所以如果只是西文字体的话，我并不很建议你这样做。

#### 中英混排

现在我们就可以使用任意的字体了。不过这也未必好用，比如我们需要对中英文使用不同的字体，就必须对每段中文设置特殊的字体，这在中英混排的环境下是非常反人类的，而且也有违 LaTeX 的哲学。这时候我们就需要 CJK 这个包，这个包的名字是 Chinese, Japanese 和 Korean 的首字母缩写，就是用于处理东亚文字相关的问题。不幸的是，CJK 并不兼容 XeTeX。好在我们有 XeTeX 下的替代品，xeCJK。我们只需要这样写，就可以分别指定不用语言的字体：

```LaTeX
\usepackage{xeCJK}    % CJK 包
\setCJKmainfont{宋体}    % 设置中文字体
\setmainfont{Times New Roman}    % 设置西文字体
```

这就是我目前对于中英文字体的解决方案，基于 XeTeX 内核， tex live 发行版。在这个问题上，不同的组合可能意味着不同的语法，我这里只说我目前的解决方案，暂时觉得是相当高效而强大的。值得注意的是 CJK 在底层也是使用了 fontspec 这个包，这也就意味着使用 CJK 也会导致最后的文档和 fontspec 一样，丢失一些 LaTeX 的排版特性。最后再附加一个我目前推荐的中文字体搭配，中文粗体用的是方正大标宋，中文正文是方正宋三。这样搭配的好处一个是中英文的字重相当，另外就是中文的粗体也能清楚的区分（因为很多中文字体是没有粗体的）。代码如下：

```LaTeX
\usepackage{xeCJK}
\setCJKmainfont[BoldFont={方正大标宋简体}]{方正宋三简体}
\setmainfont{Times New Roman}
```

这个设置对中文正文使用了方正宋三，这个字体在字形上比宋体要好看，而且字重也要更重一些，和 Times New Romam 基本对等。考虑到相当多的中文字体其实是没有粗体的（包括这里的宋三），所以我手动设置了方正大标宋，作为中文粗体效果还算比较和谐。当然要使用这个配置，你的电脑里需要先安装好上边提到的两个字体。这里因为我手上并没有大量 LaTeX 的中文文本，就不给出效果图了，如果你自己去尝试一下，你会回来点赞的（笑）。

## 图片排版

这个问题我表示有点难说，实际上用 LaTeX 排版图片是有点蛋疼的。LaTeX 不像 word，图片一拉就能看到效果，而是要不停的调整一些参数，再重新编译（编译需要的时间足够你撒泡尿或者泡杯茶了）。另外呢，LaTeX 对于图片位置安排有点难以表达，在不使用其他包的情况下，你可以控制的参数非常少，只有 “当前位置，页面顶部，页面底部” 等几个可选位置，如果你在一个页面里有若干张图，很有可能会全部堆积到后面几页。实际上图片排版在 word 里要排的好看也并不方便，或者说图文混排对于任何一个排版软件都并非易事。而使用 LaTeX 进行图文混排的优势在于便捷的文内引用以及图片的编号和标题设置。

我这里呢提供两种解决方案：一个是使用 wrapfigure，也就是类似于 word 的浮动图片；而另一种，就是文本分栏，这虽然不是一个图片排版的方法，但是可以在相当大的程度上缓解图片排版的困境。

#### 原生插图环境

毕竟是新手入门向的内容，说 wrapfigure 之前我们先讲一下原版的插图语句：

```LaTeX
\begin{figure}[!h]    % 可选参数是图片的插入位置
    \centering    % 居中
    \includegraphics[width=11.5cm]{figures/circuit.png}    %插入图片
    \caption{the connection in circuit}    % 图片标题
	\label{circuit}    % 标签，用于内文引用
\end{figure}
```

效果我就不演示了，就和你能想到的最普通的图片插入方法一样。这里比较 tricky 的就是这个图片位置，实际上只要你的图片一多，不管你设置的多操心，总是会出问题。可以用的参数可以参考 [这个教程][7]，不过在你真的遇到问题之前（主要是图片相当多的时候），你可能没法理解这个设置有多蛋疼。

#### wrapfigure 包

相比原生插图环境，这是一个比较强大而且可控参数比较的宏包，它允许你将图片嵌入在文本内部，尤其适合一些小的示意图的摆放。如果要使用的话，先通过 `\usepackage{wrapfig}` 来导入。在需要插入的位置，通过 wrapfigure 环境来插入图片，格式如下：

```LaTeX
\begin{wrapfigure}{行数}[位置][超出长度]{宽度}<图形>
\end{wrapfigure}
```

示例代码：

```LaTeX
\begin{wrapfigure}{r}{3.5cm}
  \vspace{-15pt}    % 对应高度1
  \includegraphics[width=3.5cm]{figures/74HC165.jpg}\\
  \vspace{-15pt}    % 对应高度2
  \caption{74HC165}
  \vspace{-15pt}    % 对应高度3
\end{wrapfigure}
```

效果大概是这样：

![wrapfigure 示例][4]

这里我只给出最简单的用法。需要更细致的调教的话，可以参考 [知乎上的这个问题][2]；对于位置参数，可以参考 [这篇文章][3]。这样看效果其实还是相当不错的，但是 wrapfigure 插图片有一个最大的问题，就是不兼容列表环境。我之前的版本是会出现一些鬼畜的图文重叠，只能手动调整，现在的版本直接就已经塞不进去了。所以我这里就不做演示了。

#### 分栏

对我而言，解决图片问题的最好方法应该就是分栏配合原生的插图语句。一方面因为这样可以让你可用的插图位置直接翻倍，很大程度上避免了图片拥挤的问题；另一方面它也使得文字的排布更加紧凑，阅读更加轻松。对于分栏，你只需要在声明文本类型的时候这样写：

```
\documentclass[12pt, twocolumn]{article}
```

这样出现的效果就是 LaTeX 的默认分栏布局。与此同时，你还可以通过其他命令修改这个效果，下边给出我使用的参数：

```LaTeX
% 这里是设置页边距和文本间距
\setmarginsrb{1.5 cm}{2 cm}{1.5 cm}{2 cm}{1 cm}{1.5 cm}{1 cm}{1.5 cm}
\setlength{\columnsep}{1cm}    % 栏间距
\setlength{\columnseprule}{0.03cm}    % 分割线宽度
```

这里稍微插一张图看一下效果。

![multicolumn demo][5]

这样的话，即使你有很多图，用起来基本上还是游刃有余，另外也不会有空白。于是问题来了，如果我要插入一些很大的图，需要占满整个页面，应该怎么做呢？方法也相当简单，只要加一个星号就可以，像这样：

```LaTeX
\begin{figure*}[!ht]    % 重点就是这个 figure*
    \centering
    \includegraphics[height = 0.5\textwidth]{clockCircuit.png}
    \caption{Circuit connection}
    \label{circuit}
\end{figure*}
```

效果是这样：

![multicolumn figure demo][6]

另外呢，如果～有一些文本不想分栏，比如说代码。只需要在前边写`\onecolumn`，就可以实现局部的不分栏，非常方便。

## 结

这次就说这些了，基本是我目前使用 LaTeX 觉得比较实用的一些小技巧。如果你真的跑看看这篇文章的话，希望能帮到你；另外，这篇文章更像是我个人的学习笔记，记不得的时候可以找出来参考一下（笑）。

Have fun with LaTeX!


[1]: /img/posts/about-layout-in-latex.markdown_1.png
[2]: https://www.zhihu.com/question/26837705
[3]: http://blog.sina.com.cn/s/blog_54e805300100ky4o.html
[4]: /img/posts/about-layout-in-latex.markdown_2.png
[5]: /img/posts/about-layout-in-latex.markdown_3.png
[6]: /img/posts/about-layout-in-latex.markdown_4.png
[7]: http://www.ctex.org/documents/latex/graphics/node64.html
