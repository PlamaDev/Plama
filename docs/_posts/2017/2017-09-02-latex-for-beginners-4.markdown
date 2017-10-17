---
layout: post
title: 智障的 LaTeX 入门教程（四）
date: '2017-09-02 00:28'
author: Author
catalog: true
tags:
  - LaTeX
  - Atom
---

说来惭愧，没想到这么几篇小玩意儿我竟然拖了这么久。原本打算俩礼拜更完竟然硬生生被我拖成了月番。说来无奈，我今年暑假虽说非常长，但是一方面我总是要抽时间折腾一些申研的事情，另一方面我自己开的坑实在太多（相当一部分是垃圾）。

我们之前已经把最主要的部分说的差不多了，上一篇结尾已经开始说一些细枝末节的东西了。这一篇我们继续说 LaTeX 很重要的一部分，但是如果你只是想做一个普通用户，拿模板来直接用的话，你大不用费神读这一部分了，不过你如果想自己写模板，或者在写论文时自动化处理一些操作的话，这一节还是有点意思的。

不过我这里要说一件很重要的事，就是 LaTeX 的语法是非常多的，除非专攻 LaTeX 的人，否则几乎不可能把各种命令都搞清楚。我之前说的那些只能帮助你理解大部分模板的原理，要对他有更深入的理解的话，读别人的模板绝对是相当有效的学习手段。尤其是你可以发现好多有用的包。同时，[TeX StackExchange][1] 绝对是解决你问题绝佳的网站。

说了这么多废话，我们还是进入正题吧，我们这次主要讲 LaTeX 中，值与命令的定义。附带一些 `listings` 包的配置分享。

## 值与命令

#### 调用格式

其实在不同的宏包中，作者可能会自己定义命令的调用方式（参见listings），不过这些就不是我们关心的内容了，我们这里只说最简单的情景。一个典型的 LaTeX 命令可以有一个可选参数，和 n（n>=0）个必选参数。调用一个命令的典型操作是这样的：

```latex
\afunction[optional]{compulsory1}{compulsory2}
```

方括号指代可选参数，花括号指代必选参数。当然，方括号内既然是可选参数，你完全可以不提供，就像这样：

```latex
\afunction{compulsory1}{compulsory2}
```

#### 值定义

命令的使用自然是很简单的，定义就稍微复杂一些了。我们先说值的定义，用 `\newcommand` 这个命令就可以：

```latex
\documentclass{article}
\newcommand{\name}{Towdium the Stupid}

\begin{document}
This guy call himself \name .
\end{document}
```

编译结果：

```
This guy call himself Towdium the Stupid.
```

#### 命令定义

命令定义就稍微复杂一点。我们这里现设定一个情景吧。我们平时插入独立的公式有两种方法，第一种是通过环境：

```latex
\begin{equation}
  \mathcal{L}_t{f}=\int^\infty _0 e^{-st}f(t)dt
  \label{eq}
\end{equation}
```

第二种是用美元符号：

```latex
$$\mathcal{L}_t{f}=\int^\infty _0 e^{-st}f(t)dt$$
```

你也许对于这种设定有点纠结：第一种语法很繁琐，第二种不能使用内文引用（label）。所以我们有没有办法建立一种语法简洁的公式插入格式，同时又支持内文引用呢？这时候你就可以参考下面给出的文档。当然，最好的方法是自己拿去编译一下。

```latex
\documentclass{article}
\usepackage{ifthen, amsmath}

\newcommand{\eq}[2][]{
  \ifthenelse{\equal{#1}{}}{ %
    \begin{equation*} #2 \end{equation*} %
  }{ %
    \begin{equation} #2 \label{#1} \end{equation} %
  }
}

\begin{document}
Now you can create an equation with inline cite like equation \ref{eq}.

\eq[eq]{\mathcal{L}_t{f}=\int^\infty _0 e^{-st}f(t)dt}

But you can also use it for normal equations:

\eq{\mathcal{L}_t{f}=\int^\infty _0 e^{-st}f(t)dt}

All super easy!
\end{document}
```

我知道有心编译的人毕竟是少的，所以，我这里给出结果：

![equation example][2]

这样我们便成功地解决了问题，语法简单，并且同时支持含引用和不含引用的内容。当然你可能一头雾水，我们就来解释一下这一段代码。主要内容在第四行开始，其余部分均是非常普通的语法。这里我们新建了一个命令为 `\eq`。第一个方括号我们声明这个命令有两个参数，第二个方括号是可选参数的默认值，后面则是命令体了。

后面的 if 判断来自于 `ifthen` 宏包。大致相当于：

```
if #1 == '':
  \begin{equation*} #2 \end{equation*}
else:
  \begin{equation} #2 \label{#1} \end{equation}
```

这里 `#1` 指代第一个参数，即可选参数，默认值为空。`#2` 指代第二个参数，也就是实际的公式内容。LaTeX 中，命令的参数从1开始编号，如果有可选参数，则可选参数为1，必选参数后移；如果没有可选参数，则必选参数从1开始计数。这里命令的内容是：如果可选参数为空，即不需要内文引用，则执行前一种不带序号的格式；如果提供了引用的 label，则执行后一种带 label 声明的格式。

你可能注意到了里边有几行末尾有迷之 `%`。这是因为定义的内容会被原封不动的替换到原来文本的位置上，这里行末有换行符，调用之后可能就会造成多余的换行。在行尾加上 `%` 之后相当于把这个换行符给注释掉了，也就自然不会有多出的换行符了。

## 文件导入

说完命令定义之后，可以看到定义命令确实给我们提供了一些便利，但是在导言部分多出了一大坨命令定义，可以说是非常让人难受了。我们自然有办法解决这种问题，方法就是使用文件导入。类似于 C 语言的 `#inclide`。我们可以这样搞

```latex
\documentclass{article}
\usepackage{ifthen, amsmath, filecontents}

\begin{filecontents}{header.tex}
\newcommand{\name}{Towdium the Stupid}
\end{filecontents}

\input{header}

\begin{document}
This guy call himself \name .
\end{document}
```

我这里使用了 `filecontents` 环境把两个文件写在一起了，实际上你可以把你的各种函数定义转移到另一个文件里，需要时将其导入即可。使用 `\input{文件名}` 就可以，这里不写后缀名似乎没有问题。

## 带你折腾 listings

介绍了文件导入之后，你就可以开始折腾自己的配置文件了，不管是什么模板，只要在需要时导入相关内容，就可以获得各种预先定义的特性。我平时用的模板也在 [教程第二篇][3] 的末尾给出了，需要的话可以自取。当然我并不可能把可以折腾的东西说的面面俱到，我这里只说 `listings` 的折腾内容，大概展示一下可以达到什么效果。有工夫的话，不妨编译一下下面这一段代码。其中我使用了大量的自定义格式，代码渲染终于可以有所改观了。


```latex
\documentclass{article}
\usepackage{color, listings}

\definecolor{mygray}{rgb}{0.5,0.5,0.4}
\definecolor{mygreen}{rgb}{0,0.7,0.5}
\definecolor{myorange}{rgb}{1.0,0.4,0}
\definecolor{mylilas}{rgb}{0.8,0.3,1.0}

\lstdefinestyle{mybase} {
	breaklines=true,
	showstringspaces=false,
	basicstyle=\small\tt,
	frame=single,
	xleftmargin=20pt,
	xrightmargin=20pt,
  numbersep=9pt,
  numberstyle={\color{black} \footnotesize \sf},
  stringstyle=\color{myorange},
  keywordstyle=\color{mygreen},
  commentstyle=\color{mygray}\ttfamily
}

\lstdefinestyle{mycpp} {
  style=mybase,
  language=C++
}

\lstdefinestyle{myjava} {
  style=mybase,
  language=java
}

\lstdefinestyle{myahdl} {
  style=mybase,
  language = ahdl,
  keywordstyle=\color{mylilas},
  commentstyle=\color{mygreen}
}

\lstdefinelanguage{ahdl}{
  morekeywords={
    AND, ASSERT, BEGIN, BIDIR, BiTS, BURIED,
    CASE, CLIQUE, CONNECTED_PINS, CONSTANT,
    DEFAULTS, DEFINE, DESIGN, DEVICE, DIV,
    ELSE, ELSEIF, END, FOR, FUNCTION,
    GENERATE, GND, HELP_ID,
    IF, INCLUDE, INPUT, IS, LOG2,
    MACHINE, MOD, NAND, NODE, NOR, NOT,
    OF, OPTIONS, OR, OTHERS, OUTPUT,
    PARAMETERS, REPORT, RETURNS,
    SEGMENTS, SEVERITY, STATES, SUBDESIGN,
    TABLE, THEN, TITLE, TO, TRI_STATE_NODE,
    VARIABLE, VCC, WHEN, WITH, XNOR, XOR,
    CARRY, CASCADE, CEIL, DFFE, DFF,
    EXP, FLOOR, GLOBAL, JEFFE, JKFF,
    LATCH, LCELL, MCELL, MEMORY, OPNDRN,
    SOFT, SRFFE, SRFF, TFFE, TFF, TRI,
    WIRE, X
  },
  sensitive=false,
  morecomment=[l]{--},
  morecomment=[s]{\%}{\%},
  tabsize = 4
}

\begin{document}

\begin{lstlisting}[style = mycpp]
/* Hello World in cpp */
#include<stdio.h>

main()
{
  printf("Hello World");
}
\end{lstlisting}

\begin{lstlisting}[style = myjava]
// Hello World in java
public class HelloWorld {

    public static void main(String[] args) {
        System.out.println("Hello, World");
    }
}
\end{lstlisting}

\begin{lstlisting}[style = myahdl]
SUBDESIGN dec_count
(
	enc, ent, clk	:INPUT;
	clear				:INPUT;
	value[3..0]		:OUTPUT;
	rco				:OUTPUT;
)

VARIABLE
	count[3..0]		:DFF;

BEGIN
	count[].clk = clk;
	value[] = count[];
	IF (clear) THEN
		count[].d = 0;
	ELSIF (enc & ent & (count[].q != 11)) THEN
		count[].d = count[].q + 1;
	ELSIF (enc & ent & (count[].q == 11)) THEN
		count[].d = 0;
	ELSE
		count[].d = count[].q;
	END IF;
	rco = ((count[].q == 11) & ent);
END;
\end{lstlisting}

\end{document}
```

抱歉这段代码有点长，只是为了展示更多内容。你可以看到这里我们自己定义了 C++，Java 的语法高亮，甚至添加了 AHDL 的支持，并自定义了高亮。你可能没有听说过 AHDL，这并不重要，你只要当他是某门快要消失的语言就好了。编译结果如下：

![listing example][4]

这里 `\lstdefinestyle` 用于自定义自己的格式，`\lstdefinelanguage` 用于定义新的语言。其余的我也就不多作解释了，如果你有这方面的需要，网上可以找到很多相关的资料。或者你可以删减定义中的某些项，观察其中的影响。我这里只是给出一个具体的例子，展示一下 LaTeX 的自定义程度。

## 结

如果有幸你读到这里，LaTeX 主体的介绍就告一段落了。基于这几篇的内容，你应该可以比较轻松的阅读，写作和管理 LaTeX 的文档了。不过我本人也在不断学习 LaTeX，前几个月我的最新成果是纯用 LaTeX 绘制了一大波电路图，效果拔群。当然这就是十分细枝末节的内容了，我大概不会有工夫拿到这里来讲。当然，这并不意味着 LaTeX 的教程就不会更新了，我们有缘再会。

Fun with LaTeX!


[1]: https://tex.stackexchange.com/
[2]: /img/posts/latex-for-beginners-4_1.png
[3]: www.towdium.me/2017/07/10/latex-for-beginners-2/
[4]: /img/posts/latex-for-beginners-4_2.png
