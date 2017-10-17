---
layout: post
title: 智障的 LaTeX 入门教程（二）
date: '2017-07-10 17:32'
author: Author
catalog: true
tags:
  - LaTeX
  - Atom
---

## 前言

在搭建好环境（也许）之后，小伙伴们大概已经跃跃欲试地想搞点事情了。这一篇会教大家 LaTeX 代码的整体逻辑，并非常粗略地介绍一些常用结构的用法。前方加速，请小伙伴们抓好扶手。

## 你的第一个 LaTeX 文档

这里我们要做的事情很简单：新建一个文件夹，新建一个后缀为 `.tex` 的空文本文件，然后填入下面的代码：

```latex
\documentclass{article}

\begin{document}
Hello \LaTeX
\end{document}
```

然后，执行这个命令 `Latex: Build`（hint: `ctrl+alt+P`），如果一切正常，你就可以在上一篇中你配置的位置找到输出文件了。在我的环境中，编译效果如下图：

![Build result 1][1]

在 `out` 文件夹中（上一篇中我自定的位置），就可以找到我们要的 pdf 文件了。如果你已经安装了 `pdf-view` 插件，就可以直接打开了。之后的每次编译，内容也会相应的更新。

#### 重要语法

说完操作步骤，我们稍微说说原理。你可以看到这一段文本有很多转义符（反斜杠）。在 LaTeX 中，这用来表示一个命令。配合编辑器的高亮，你可以轻松地分辨出各个部分。对于普通的指令，一般是这样的格式 `\command[arg1]{arg2}{arg3}` 前面的一部分是指令名，方括号中是可选参数，花括号中是必选参数。对于最基础的命令来说，至多有一个可选参数，任意个必选参数。当然，对于一些简单的命令，没有参数也是完全可以的。比如这里的 `\LaTeX`，就只是表示了一段特殊文本。在我们深入学习之后，完全是可以自定义命令的，就像各种编程语言中自定义函数一样。

另外一个非常重要的概念是这里的环境，也就是 `\begin{document}` 和 `\end{document}`。环境的范围由 `\begin{env}` 和 `\end{env}` 来确定。在这一范围中的文本会有特殊的效果。这里的环境 `document` 表示着文本的内容范围，在此之外是不允许出现文章内容的，你可以用那些位置来定义各种格式，或者配置各种宏包。除此之外，还有巨量的环境可以使用，比如文本对齐的 `align`， 插入公式的 `equation`，插入源代码的 `lstlisting`。他们可能会重新定义环境内文本的显示方式，充满着玄学，但是很好用。

#### 内容解释

这里的 `\documentclass{article}` 是每个 LaTeX 文本必备的部分。这里相当于你告诉编译器这个文本是一个文档，那么编译器就会用文档的格式来编译。除此之外，常用的还有 `\documentclass{book}` 这就意味着这是一本书籍的排版。在这种情况下，编译器可能会考虑更大的标题，更多分页。具体效果取决于 LaTeX 内嵌的排版格式。当然格式都是可以魔改的，不过这就略玄学了，我们先不说。`document` 环境里我们就打了一行字，正如你在 pdf 文件中看到的那样。

## 你的第二个 LaTeX 文档

大概知道他的原理之后，我们不如激进一点，下面是整个文档的源码，你可以着手准备编译了。

```latex
\documentclass[11pt]{ctexart}

%导入宏包
\usepackage{esint,bm}       %提供环路重积分符号和数学粗体
\usepackage{indentfirst}    %修改首行缩进的行为
\usepackage{fullpage}       %两边边距填充
\usepackage{graphicx}       %插图
\usepackage{listings}       %插代码

%配置宏包
\renewcommand{\lstlistingname}{代码}       %重定义代码命名

%文档信息
\title{一篇 \LaTeX{} 入门介绍}
\author{Towdium}

\begin{document}

% 绘制标题
\maketitle

% 正文内容
\section{数学公式}

这是一段示例公式，最简单的方式是使用两个美元符号，像这样：

$$\oiint \bm{E}\cdot d\bm{s}=\frac{Q}{\epsilon_0}$$

对于内文公式，还可以这样写：$E=mc^2$。

当然，你也可以用更复杂的方法，如公式\ref{eq}。
好处在于这种用法可以对公式添加序号，以便进行内文引用。

\begin{equation}
  \mathcal{L}_t{f}=\int^\infty _0 e^{-st}f(t)dt
  \label{eq}
\end{equation}

\section{图片插入}

通过 \verb+graphicx+ 这个包，我们可以非常轻松的插入图片。
图\ref{fig}是一张示例图片，同样使用了内文引用。当然插图的方法还有很多，
来实现不同的效果，我们这里只讲基础。这里的图片是浮动的，在没有位置的时候，
他可能会浮动到其他页面上，这样可以防止文档里出现大量的空白。

\begin{figure}[!ht]
  \centering
  \includegraphics[width=0.4\textwidth]{pic}
  \caption{一个示例图片}
  \label{fig}
\end{figure}

\section{代码插入}

这里提供了一个最小化的代码插入方式。效果给出在代码\ref{code}中。
包 \verb+listings+ 提供了无比强大的代码高亮支持，
只不过我这里没有进行相关的设置，所以内容均是黑白的。
有机会的话我会在后文中提供一些思路。

\begin{lstlisting}[language=python, frame=single,
caption=一段示例代码, label=code, captionpos=b,
basicstyle=\footnotesize]
usage = "Run the script: ./geolocate.py IPAddress"
if len(sys.argv)!=2:
    print(usage)
    sys.exit(0)
\end{lstlisting}

\section{表格插入}

老实说表格在 \LaTeX{} 中的使用比较反人类，尤其当你想要合并单元格的时候。
不过这也是没有办法，任何一个富文本语言对表格这种东西基本上都相性不合。
表格\ref{tab}提供了一个较为简陋的表格实现。

\begin{table}[!h]
  \centering
  \begin{tabular}{ c | c | c }
    \hline
    cell1 & cell2 & cell3 \\ \hline
    cell4 & cell5 & cell6 \\ \hline
    cell7 & cell8 & cell9 \\  \hline
  \end{tabular}
  \caption{一个示例表格}
  \label{tab}
\end{table}

\section{枚举}

枚举什么的其实也很简单，大概像这样：
\begin{enumerate}
  \itemsep-0.5em %缩小间距
  \item 第一条
  \item 第二条
\end{enumerate}
在枚举环境中，他也会完全帮你搞定编号排序的问题。
当然，你也可以使用没有编号的版本：
\begin{itemize}
  \itemsep-0.5em %缩小间距
  \item 第一条
  \item 第二条
\end{itemize}

\end{document}
```

#### 排错

当然这个文档在你的电脑上很可能不能成功编译。有下边几个原因：

- 你没有使用 XeLaTeX 引擎：看前一章我们说的引擎配置。
- 你没有图片 `pic.pdf`：我已经上传到了 [Github 仓库][5] ，下载并放置到源文件的同一文件夹，并命名为 `pic.pdf` 即可。当然你如果想用自己的图片也没有问题，`\includegraphics[width=0.4\textwidth]{pic}`中的 `pic` 就是插入图片的路径。我个人建议 pdf 图片，他长期以来发挥稳定，其它格式有时则会翻车。你可能还要尝试带后缀名和不带后缀名的写法。

对于新手而言，阅读 LaTeX 的错误日志是相当痛苦的事情。我刚开始学的时候，基本上每次编译都是几十个警告，只能硬着头皮写，当你逐渐熟悉之后，你才能明白这些警告在说什么。当然最好的方法是找一些模板或者非常短的文章来编译，从一开始就不要允许大规模警告的出现。

#### 简要讲解

如果你能成功编译的话，我觉得我也没必要说太多了，各个排版结构的效果和源码你都能看到，重要的命令我也给出了注释。而某一些命令的作用，你完全可以删除特定代码来观察结果的变化。

关于内文应用，他的逻辑是在某一个部件的内部定义 `\label{foo}`, 然后在文档的任何地方引用只需要 `\ref{foo}` 即可。因为 LaTeX 会编译多次，所以即使先引用后定义也是完全没有问题的。

关于中文的显示，这里我用的是 CTeX 宏包进行的配置。不得不说确实非常好用，配置简单，发挥稳定。你只需要在第一行使用 `ctexart` 即可。

关于段落和分词，LaTeX 以两个换行作为段落分割，一个换行没有任何作用，只是方便代码排版。如果要强制换行，可以使用 `\\`。类似地，LaTeX 也会自行处理单词间的间距。无论使用多少个空格，LaTeX 只会把它当作单词分割，输出的结果都是一样的。

如果你之前习惯使用 markdown 的话，可能会难以接受 LaTeX 代码的复杂度。本质上来说，markdown 代码只含有文本内容，而没有特定格式，但是 LaTeX 是完全不同的，他的强大之处在于你可以自定义任何的一个格式，对于一些常用的代码块，你完全可以自定义成函数，这样就可以大大减小冗余文本的数量。现在你大概可以知道，LaTeX 和 markdown 完全不是一个数量级的产品。

当然每个命令可以配置的选项都是异常复杂，我这里只是给出最简便的方法。如果想要最完整的介绍，你可以找到各个宏包的官方文档，基本上都是几十页起步。对于中文文本格式和插入图片的细节，我在 [这篇文章][2] 里做过一些介绍。当然 LaTeX 可以挖掘的内容几乎是无限的，我所介绍的内容仅仅是我日常用到的范畴。如果对任何一个细节有更高的要求，你都可以在搜索引擎上找到更具体的介绍，我这里只提供方法，不提供细节。

考虑到有些人可能还是难以编译，或者懒得编译，这里我给出我的编译结果，图看不清的话可以右键查看原图：

![Build result 2][3]

如果你有任何编译的困难，也欢迎在评论区（需要科学上网才能显示）发给我。

## 模板

在用了这么久 LaTeX 之后，老实说我没有一次是从零开始码起的，绝大部分情况下，都是基于模板进行工作。我们上次说到的 [overleaf][4] 就是一个很好的模板下载网站，你可以搞到成吨的模板。如果是作业的话，学校一般会提供 LaTeX 模板来统一画风，所以直接那它来改就好。这里再贴一个我自改自用的模板，他其实已经深度魔改了，带有一百多行的导言，包括各种函数定义以及大量高亮语法的定义，另外还有一个自改的 IEEE 引用格式，至于引用的部分，我们会在下次说到。这个模板我也发到 [Github 仓库][6] 了，有需要的可以自行下载使用。

## 结

这次我们给出了各种常见部件的语法（当然都是最基础的方式），下一次我们会着重说文献引用。那我们一会儿见。


[1]: /img/posts/latex-for-beginners-2_1.png
[2]: http://towdium.github.io/2016/08/16/about-layout-in-latex/
[3]: /img/posts/latex-for-beginners-2_2.png
[4]: https://www.overleaf.com/
[5]: https://github.com/Towdium/towdium.github.io/blob/master/share/latex-for-beginners_pic.pdf
[6]: https://github.com/Towdium/towdium.github.io/blob/master/share/latex-for-beginners_template.zip
