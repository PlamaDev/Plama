---
layout: post
title: 智障的 LaTeX 入门教程（三）
date: '2017-08-06 20:26'
author: Author
catalog: true
tags:
  - LaTeX
  - Atom
---

## 前言

之前我们说了一些基础语法。说好了一会儿见结果一拖就是大半个月。这其中有相当一部分原因是我中途被我爸拖出去强行旅游了一个礼拜还带了几天孩子。当然这不重要，我们继续。这次我们说一些基础的引用语法。

## 何为引用

说真的如果你还不清楚引用是啥，你可能不需要看这一章。不过你既然来了，我们就稍微说一说。粗略地说，当你在论文里引用了其他人说的话，你不能像我写博客这么随意，直接丢个超链接，取而代之的是，你需要在论文结尾提供这个来源的详细信息，诸如作者，年份，文章标题，或者是期刊刊号，图书页码。你需要在文章末尾的参考文献目录中列举这些信息，以及在对应地在文本里添加注释。下图我给出了我某一篇论文的参考文献目录，当然内容不重要（不仅不正式，甚至还有错别字），大概就长这样。当然是用 LaTeX 直接生成的。

![Example bibliography][1]

说到这里我不禁要婊一婊 word，你在 word 里很难把参考文献排的这么规整，特别是你有一些很鬼畜的超链接时。这个例子我们使用了 IEEE 格式，如你所见，一个典型的 IEEE 引用格式是：

> \[序号\] 名首字母.姓全拼, “文章名,” 年份. 其他信息.

参考文献还有各种不同的格式，这里给出的 IEEE 是理工科常用的，而人文学科多会使用 APA 或者 Harvard 格式。这些格式基本大同小异，按照作业要求选取就可以了。不同引用格式的区别你可以在 [这里][2] 找到。当然引用的具体格式你根本不用知道的那么清楚，LaTeX 会帮我们直接生成。不管你用什么格式，我们只需要给 LaTeX 提供必要的字段，他就可以帮我们生成需要的格式。

除去参考文献列表，文本内的引用格式也是各不相同的，你要做的同样是告诉 LaTeX 你需要的格式，他会搞定一切。

## 操作方法

#### bib 文件

LaTeX 使用 BibTeX 文件来管理引用列表。要添加引用，最简单的方法是在 tex 文件的同一目录下新建一个文件，名为 `reference.bib`,内容大概像这样：

```bibtex
@misc{example,
  author = {Towdium},
  title = {An Example Article},
  year = 2017
}
```

抱歉我这里用的高亮插件 rouge 不支持 bib 语法，但是在 atom 里他是可以正常高亮的。

- 第一行的 misc 表示这是一条杂项引用，他表示了你这个引用是书籍，文章还是杂项。
- 括号的开始是一个标识符，用于引用内容的识别，相当于这个条目的名字。
- 后面是条目的各个字段。不同的引用类型对应不同的字段。杂项引用只需要这三条即可。
- 这里的逗号非常重要！逗号错误会导致神秘的编译失败。

同一个文件可以容纳同一片文章里所有的引用信息，只要列举在同一个文件里就可以。

#### 正文引用

当你完成这一个条目之后，你要做的就是在正文里引用这个条目。正文内容可以是这样的：

```latex
\documentclass{article}

\begin{document}

Towdium wrote a stupid article about \LaTeX{} \cite{example}.
\bibliographystyle{IEEEtran}  % set style to IEEE
\bibliography{reference.bib}  % set reference file name

\end{document}
```

将这两个文件放在同一文件夹进行编译，你得到的结果大概是这样：

![Example bibliography][3]

值得注意的是正文中的 `\cite{}` 命令就是用来引用文献的，这里的参数就是我们之前提到的标识符。下边的 `\bibliography{}` 命令提供了源文件的文件名。执行引用时，他会到这个文件里寻找相关的条目，然后添加进来。

这里的 `IEEEtran` 是 LaTeX 自带的 IEEE 引用格式，不过如果你安装的是精简版的 TexLive 的话，也许会没有这个格式。在 archlinux 下边，你需要安装 `texlive-publishers` 这个包，其他系统还请自行解决。

## 操作细节

#### 引用类型

BibTeX 提供了相当多的模板来帮助你填充引用信息。最简陋的有 misc （杂项），你可以用它来做网页的引用。除此之外，它还提供诸如学术文章，书籍，会议记录，手册等各种引用模板。不同的模板有会用到不同的字段，这些信息在 [Wiki 的 BibTeX 页面][4] 上都有比较详细的解释。最简单的是杂项，没有必选字段，你给什么信息他就填充什么信息；而正式一点的 Article 之流，则必须提供作者，标题，期刊名，年份，可选的字段有：期数，文章编号，页码和额外注释。你可以在各种模板中选取你需要的来使用。文档编译后，各种模板的信息都会被转化对应的引用格式。

#### 自动生成 bib 文件

严格来说这并不是 LaTeX 的技巧，只能算是写论文时候偷懒的办法。你完全没必要手动写 bib 文件，取而代之的是，大部分数据库都会提供引用导出的功能。

![ref export][6]

这里拿 google scholar 作为例子。点击条目下的“引用”就会弹出图示界面；点击 bibtex，就可以直接导出成 bibtex。其他数据库，其他文献管理程序同样适用。大数据库导出的文件一般比较可靠，但是也有一些需要手动微调。

#### URL

严谨地说 URL 跟引用并没有什么关系，我之所以在这里说，是因为相当一部分引用都会自带一个超长的超链接。如我之前所说，超链接这种东西你很难把它排的好看，因为大部分的排版工具都难以决定如何分割这么长的文本。LaTeX 的好处就在于它把超链接处理成另外一种文本，甚至用另外一个包来管理超链接相关的排版，这就给我们的暴力切割提供了机会。可喜可贺的是，url 包甚至可以自动解决排版问题。

你可以考虑编译以下的内容：

```latex
\documentclass{article}
\usepackage{lipsum}
\usepackage[hyphens]{url}
\usepackage[hidelinks]{hyperref}

\begin{document}
\lipsum[1]

Here is an example link: \url{http://assume-this-is-a-super-duper-long-url.
let's-give-it-a-stupid-domain.com} Maybe you can
find something there

\lipsum[2]
\end{document}
```

这里我用 lipsum 包只是为了展示普通段落的宽度。重点在第三行。当你选择了 `hyphens` 参数的时候，url 包就会接管换行的操作。第四行的 `hyperref` 包可以将 url 转换成超链接，完全是自动的。不过记得加上 `hidelinks` 不然会有略难看的彩色方框。效果如下：

![url demo][5]

当你使用这一配置的时候，参考列表中的 url 也会遵循同一模式，效果大概就像我一开始给出的图那样。

#### filecontent

和上一小节一样，filecontent 也是 LaTeX 的一个包，我们把它放在这里说只是因为在引用的范例中经常会看到他。filecontent 这个包用于编辑其他的文件，或者说，在一个文件中伪造出其他的文件。你可能要说了，这不就是脱裤子放屁么。在某些情况下，是的，但是在 bibtex 的使用上，他能提供一些便利。

我们之前说了 bibtex 需要存放在另一个 `.bib` 文件中。但是对于短文件或者示例而言，写在一个文件里可能更方便。这里 `filecontents` 环境就是用来定义另一个文件的内容。这里我们通过他建立了 `reference.bib` 这个文件。

```latex
\documentclass{article}
\usepackage{filecontents}

\begin{filecontents}{reference.bib}
@misc{example,
  author = {Towdium},
  title = {An Example Article},
  year = 2017
}
\end{filecontents}

\begin{document}
Towdium wrote a stupid article about \LaTeX{} \cite{example}.
\bibliographystyle{IEEEtran}  % set style to IEEE
\bibliography{reference.bib}  % set reference file name
\end{document}
```

直接编译以上的代码，可以获得和之前的示例完全一致的效果。这样就省去了我们操作文件的麻烦。

#### biblatex

我们一直在讲的都是最最基础的 LaTeX 的引用方式。他的局限性还是相当大的。你可以人为操作的配置也比较有限。包 `biblatex` 提供了更为强大的自定义余地，同时也提供了更多的条目模板。下边是一个用 biblatex 进行引用管理的例子。

```latex
\documentclass{article}

% sometimes it won't work without setting bibtex
\usepackage[backend=bibtex, style=ieee]{biblatex}
\usepackage{filecontents}

% allow stretch for better hbox filling, optional
\emergencystretch=1em

% declare reference data location, no suffix
\bibliography{ref}
% override default date style to [Accessed: MM/DD/YY]
\DeclareFieldFormat{urldate}{\mkbibbrackets{Accessed: #1}}

\begin{filecontents}{ref.bib}
@online{example,
  author = {Towdium},
  title = {An Example Article},
  year = 2017,
  url = {http://www.towdium.me/2017/07/10/latex-for-beginners-1/},
  urldate = {2017-08-06}
}
\end{filecontents}

\begin{document}
Towdium wrote a stupid article about \LaTeX{} \cite{example}.
\printbibliography
\end{document}
```

下面是排版的效果：

![example biblatex][7]

不得不承认使用 biblatex 应该算是更为“现代”的引用管理方式。不过就我目前的需要，使用原生的引用管理就可以满足我绝大部分的需要了。鉴于 biblatex 的资料还算比较多，使用也比较直观，我这里就只是抛砖引玉，不再细说了。

#### 魔改 bst 文件

除去 biblatex 以外，我们自然可以用最基础的方法自定义引用格式。看过最初的例子之后，当你观察输出的结果 `"An example article," 2017`，肯定会有人对这个逗号的位置无比焦虑。又或者当你想要注明在线内容的访问时间，事情就会有些尴尬。如果是个人用的话，修改 bst 文件不失为一个高效的解决方法。当然这个方法比起 biblatex 来，就要难懂得多了，而且比较野蛮。

顾名思义，bst 就是 bib style，他决定了你的引用内容的排版样式。通过修改 bst 文件，我们就可以自定义排版的格式。对于前面两个问题，我拿 IEEE 格式举个例子。首先你要找到 IEEE 格式默认的 bst 文件。我这里的位置在 `/​bibtex/​bst/​IEEEtran/​IEEEtran.​bst`。然后，复制一份到你的工作目录，按需要命名。最后，把 `\bibliographystyle​{IEEEtran}` 改成 `\bibliographystyle​{fileName}`。这样编译的时候就会使用你提供的格式文件进行排版了。通过魔改这个 bst 文件，我们就可以相应地修改引用格式。

- __逗号位置__：搜索字段 `output.nonnull`，把 `prev.status.quote quote.close` 对应的一段提到最前面即可。
- __访问时间__：搜索字段 `misc`，把 `format.note output` 放到 `format.url output` 的后边，然后在 bib 文件的对应条目添加 `note = {[Accessed: DD-MM-YYYY]},` 这一条目，就可以正确的排版了。

如你所见，bst 文件对于普通用户而言是相当难懂的，我这里只是提供一个案例，需要时可以作为参考。大部分时间你完全可以使用 biblatex 完成你的需要。

## 结

我们来觉得这一篇可以最快完成，因为他本身内容并没有太多。最后我发现我还是 too naive。当我想稍微说的深入一些的时候，就不得不考虑到读者的基础，花很大的功夫去解释一些细枝末节的操作。当然我的风格仍然是能给代码就不说话。像这种富文本语言，稍有编程经验的人应该就可以从中找出代码与文本格式的关系。我也尽量把代码写的简洁，便于读者进行编译。这篇说完了，我们的入门内容就告一段落了。下一篇我们会讨论一些略微深入的操作。比如自定义一个方法，或者编程实现流程控制。那我们一会儿见吧。

[1]: /img/posts/latex-for-beginners-3_1.png
[2]: http://pitt.libguides.com/c.php?g=12108&p=64729
[3]: /img/posts/latex-for-beginners-3_2.png
[4]: https://zh.wikipedia.org/zh-cn/BibTeX
[5]: /img/posts/latex-for-beginners-3_3.png
[6]: /img/posts/latex-for-beginners-3_4.png
[7]: /img/posts/latex-for-beginners-3_5.png
