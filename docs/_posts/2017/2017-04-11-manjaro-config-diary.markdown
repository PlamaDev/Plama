---
layout: post
title: 小白的 Manjaro 调教日记
date: '2017-04-11 21:29'
author: Author
catalog: true
tags:
  - Manjaro
  - Linux
---

## 扯个淡

之前的文章里我也提到了，我从大概半年前，绝大部分的工作（和游戏）已经转移到了 Linux 平台。我最初上手的发行版是 Ubuntu，用了几天之后出于对 unity 的厌恶，转移到了 Ubuntu GNOME。然后接下来的几个月我基本上都在用 Ubuntu GNOME。Ubuntu 对于新手而言，作为上手的发行版的确是不错的，一方面他的安装非常轻松，另一方面他的系统预先就配置好了相当多的部件，所以你也就不用接触那些稍微有点“危险”的操作，像是分区挂载，显卡驱动，系统引导这些东西，我这两天配置 manjaro 的时候经常会玩脱，搞得系统没法启动。私心说 Ubuntu 的另一个好处就是他是被 steam 官方支持的，所以游戏的兼容性也会更好一些。说起来我在操作系统上基本上就是个小白，控制台基本玩不转，脱了图形界面基本上全程懵逼，VIM 也是学得很狰狞。

但是用下来，Ubuntu 也有一些让人没法忍受的地方，比如说他的包很老旧，如果要新一点的包多半要去翻 ppa，而 ppa 这个东西，因为是分散的，所以非常头疼，而且还自带了兼容性差的 debuff。如果你是 ppa 的重度用户，估计免不了面对诸如“包XXX使用了弱安全算法”“包XXX没有发布文件”之类的控制台输出。而如果你要用官方源，他可能还停留在两三年前，可能那个版本还有一个 bug 没有修复或者少一个功能，这时候就会无比吃瘪。而 Arch 这边，感觉整个社区都在追新版本的更新包。在 Arch 的 [FAQ][1] 页面上，有这么一条“某个包已经发布了新版本，官方源什么时候会更新呢？”，下面的回答给人的感觉就是“别催别催！我们确认之后就立刻更新！”（笑）。这种感觉在 Ubuntu 社区几乎是不存在的，在 [circuitikz 的 issue 页][2] 上，作者对于一个问题，给的回复大致意思就是“哇，你在用 Ubuntu 啊，他们那儿的包可老了，你还是直接从我这儿下载吧”。

说了这么多你可能可以明白 Ubuntu 的包发布有多让人吃瘪。当然他们的意思是“我们完全可以接受老一点的包，我们更在意稳定性”。这样说来 Ubuntu 的长期支持版本的确是比较稳定，但是你可以操作的余地也很小。所以大部分人都承认 Ubuntu 是一个易于上手的发行版，但是对于深度使用，比起其他发行版还是要差一些。以上就是我跳槽的大部分原因了，另外一部分是我转移到 manjaro 之后才逐渐感觉到的，一方面继承了 Arch 超强的包管理，另一方面他在一些细节上做的也是惊人的细致，比如预制的 bumblebee，还有定制的 firefox 主题，这些我们后边还会提到。

我一开始其实是准备直接跳槽 Arch 的，但是我必须承认我高估了我的水平，从 Arch 启动的时候我就陷入了一脸懵逼，因为 Arch 根本不自带图形界面，在花了几个小时阅读 Arch 的维基之后，我果断选择了放弃。听说 Arch 的安装步骤也是故意增加了难度的，这样就可以过滤掉一大波脑残玩家（比如说我）。然后我就跳槽到了 Manjaro，这是一个深度魔改的 Arch 发行版，一方面他一定程度上继承了 Arch 的激进特性，包括包管理和滚动更新，另一方面他还算友好。Manjaro 现在官方提供预制桌面环境的安装包，包括 KDE，Xfce 和 GNOME。我本人是 GNOME 拥趸，所以就毫不犹豫的选了 GNOME 版本。然后我们下边准备说正事了。

## 下载安装

#### 安装包下载与 BIOS 设置

在官方网址就能找到他的 iso 发布镜像。有些奇怪的是他的镜像发布页还有一些奇奇怪怪的文件，结论是他们都没什么用处，只要下载 iso 文件就好，对应你需要的桌面环境版本。然后很重要的一点事，到你的 BIOS 里把启动设置调成 legacy（或者是 legacy & UEFI）。我之前的设置是 UEFI，他在安装 Ubuntu 和 Arch 原版时都很正常，但是启动 manjaro 的时候报了一个 “GRUB rescure， unknown filesystem”。我以为是因为我格了原来的 ubuntu 导致 GRUB 损坏，然后就到处折腾企图修复 GRUB （实际上根本没啥问题）。修改了启动模式之后，manjaro 终于可以正常启动了，此时我已经在烧启动盘，重装系统，修复引导的操作上折腾了一天。

#### 启动选项与安装

成功启动之后，我们就可以进入启动选项界面了（这时候还没进系统所以没有截图）。这里你可以选择语言，我个人建议用英文，这样的好处是安装好之后你根目录的几个文件夹就是英文的名称。如果你用了中文安装，那些文件夹就会是中文的，而 linux 本生对中文显示的支持还不算完美，指不定在哪些程序里边给你把中文全显示成方框（说的就是你 IntelliJ），这个时候就很吃瘪了。所以我的选择是用英文安装，当然用中文我也用过一阵子，并没有特别严重的问题。

另一个选项是驱动版本，这里你可以选择 free 或者 nonfree。我个人的建议是 nonfree。顾名思义，这个就是开源驱动可闭源驱动，如果你是原教旨主义者，我不介意你选开源驱动（而且他的兼容性更好），但是如果你要那他打游戏的话，我比较建议你选闭源，因为他在性能上要好一些，区别是肉眼可见的级别。当然这个驱动设置即使是安装好系统之后也是可以再修改的。选完之后就可以启动系统了，启动之后我们就进入熟悉的 GNOME（或者其他环境）的界面啦，然后你要做的就是一路下一步，安装完之后硬盘启动电脑，就安装完成了。

## 硬件设置

#### 显卡驱动

如果你想切换驱动的话，你只需要打开程序 manjaro settings，这里边你就可以选择你要的驱动版本。如果已经有其他版本的驱动已经安装了的话，你可能需要先把它卸载，不然他会提示 conflict，无法安装新驱动。显卡驱动我建议早些确定下来，因为我目前的感觉是调整显卡驱动会有些风险，会引起系统没法进图形界面，当然如果你是命令行大佬当我什么都没说。

然后我稍微说一下混合显卡的问题，如果不是混合显卡可以跳过。我目前的笔电是混合显卡，具体说是 i5 4200H 和 GTX 860m，在选择闭源驱动安装或者切换到闭源驱动的时候，会有几次无法进入系统，GNOME 会说“An error occured”，我目前的方法就是多重启几次电脑这个问题就自动消失了。如果系统还不稳定的话，你可以考虑安装 virtualgl 和 lib32-virtualgl。至于他究竟有什么用我也并不清楚，出处是 [manjaro wiki][3]。Manjaro 已经内置了双显卡驱动，在驱动选择界面你可以选择单显卡驱动和双显卡驱动，双显卡驱动的区别是他的名字里边带有 bumblebee，我这里叫做“video-hybrid-intel-nvidia-bumblebee”。Bumblebee 是 linux 下相当好用的双显卡解决方案，他允许你手动选择用哪个显卡打开程序。如果你想用独显打开的话，你可以用 `primusrun steam` (打个比方)来启动。或者你也可以在 steam 的游戏启动选项里打开高级选项，填写 `primusrun %command% ` 这样你就可以规定用独显来启动特定的程序。值得一提的是，在使用 bunblebee 框架的时候，nvidia 设置是不能打开的，你需要使用 `optirun -b none nvidia-settings -c :8` 来启动 nvidia 面板。其他设置依然可以参考 [manjaro wiki 页面][3]。

#### 分区挂载

这里我觉得最要紧的问题就是 manjaro 安装完之后不能写入 NTFS！也就是说他们打开只能只读。我现在这个硬盘装的是双系统，大部分分区仍然是 Windows 下边的 NTFS 格式，然后发现 NTFS 分区不能写入。当然解决方案也很简单，安装 `ntfs-3g-fuse` 这个包就好。我强烈建议你不要用 `ntfs-config` 这个包，上次安装这个包直接导致我系统各种挂载不正常，然后我就又重装了一次（捂脸）。

如果你之前用的是 Ubuntu 的话，你可能会发现 nautilus （文件管理器）左边的驱动器列表没有了。最简单的方法是在“其他位置”里能找到其他的分区然后手动挂载，但是这个方法其实不如以前直观，而我至今没有找到完全相同的修改方法，网上查的一些资料似乎说 nautilus 的驱动器列表和分区的挂载点有关系，不过看得我有点似懂非懂。我目前的方法是设置系统启动时自动挂载所有分区，相关设置可以在 `disk` 这个程序里边操作。在分区选项里选择挂载选项，打开手动挂载，选择启动时挂载，挂载路径填写 `/mnt/driverName` 这样你就可以把它挂载成一个普通的文件夹了，然后把这个文件夹添加到收藏，他就能出现在 nautilus 左侧的列表了，而且每次启动都可以自动挂载，非常方便。

## 软件配置

#### 包管理

在 manjaro 里使用了 `pamac` 作为默认的包管理器，恩，非常好用，用法基本上和 Ubuntu 的新立得差不多，但是在 Arch 下边有一个特别大的好处，就是 AUR （基本对应 ppa）是集中管理的，也就意味着你在包管理器下边可以直接搜索安装 AUR 包，也完全是一键操作。如果要使用 AUR 包，只要在设置里开启 AUR 支持即可。这里我稍微说一下 AUR 的原理（也可能不对），他基本上就是一个执行脚本，安装包的时候这个脚本就被执行了，它可以从 git 上边获得一些文档，然后执行一些编译脚本，然后把文件移动到相应的位置。AUR 的大部分包都是源码发布的，所以安装可能会花一段时间（未必），所以做好心理准备，然后泡杯茶（误）。

#### 输入法配置

目前主流的输入法框架有两个选择，ibus 或者 fcitx。实话说 ibus 在 GNOME 环境下的集成是必较好的，兼容性也比较好（至少我之前在 ubuntu 下边用 fcitx 时运行 octave 必然崩溃）。但是我目前在用的是 fcitx，主要原因是他比较好看，次要原因是它可以配置的内容，可以使用的输入法都比较多，而且最近也没出现什么我无法忍受的问题。另外，如果你特别想用搜狗拼音，那的确只有 fcitx 支持。因为我上次配置 ibus 还是好几个月前，所以我们这里只说 fcitx 的配置。

基本上你要做的就是安装这几个包：

- fcitx： 输入法本体
- fcitx-gtk2,gtk3,qt4,qt5：对于不同 GUI 框架的支持
- fcitx-cloudpinyin：云输入支持
- fcitx-skin-material：一个很不错的皮肤
- fcitx-rime：我比较推荐的输入法

这里第二行是一系列包，他们并不是必装的，但是可以改善对应框架的兼容性。的后三个包都不是核心内容，是我个人的口味。rime 这个输入法我个人的感觉是比搜狗好用。你可以稍微在知乎上搜索一下，就知道这款输入法的评价有多高，但是我没啥功夫折腾这个，就找了 [大佬给的现成的词库][4]，依然很好用。关于输入法的调教我就不细说了，我连小白都算不上，要找资料的话网上资源还是很多的。

安装完输入法，你需要把输入法添加到环境变量里才能让他生效。我目前的配置是在 `~/.xprofile` 这个文件里加上下面几行：

```bash
export GTK_IM_MODULE=fcitx
export QT_IM_MODULE=fcitx
export XMODIFIERS=@im=fcitx
```

如果你用的是 ibus 的话，把 fcitx 替换成 ibus 即可。然后注销帐号，应该就可以使用了。不过这个配置可能并不是通用的。如果这样设置对你的系统没有用的话，[Arch wiki][5] 上还提供了更细致的配置说明。

## 一些程序零碎的配置

#### Steam 配置

如果你发现你的 steam 不能正常运行，就去装 `steam-native` 这个包。然后是不是就好了？如果你玩 XCOM2 的话，你可能会发现这个游戏在 Ubuntu 上能正常运行，但是在 Arch 上卡的能吃屎。你可以考虑调整他的配置文件，位置是：

```bash
~/.local/share/feral-interactive/XCOM2/VFS/Local/my\ games/XCOM2/XComGame/Config/XComEngine.ini
```

把 `PoolSize=10` 改成 256，`OneFrameThreadLag=True` 改成 False。再运行一下，虽然还有点卡，是不是能玩了？

#### JetBrains IDE 配置

你可能会发现 JetBrains 的 IDE 会有一个颜色漂移的问题，据说是来自于 jre。解决方案你可以参考 [这篇 wiki][6]。当然，最简单的方法是在 IDE 设置里把文字抗锯齿设置成灰度（greyscale），这样的效果我觉得已经基本可以接受了。

#### GNOME 插件配置

要修改 GNOME 的插件，你需要安装 `chrome-gnome-shell`,然后打开预装的 `gnome-tweak-tool`，在相应的面板就可以编辑了。我对于 manjaro 挑插件的口味不是特别能认同，所以我把他们的大部份插件都拿掉了，按我的口味重新换了一波，大概列在下边：

- Advanced volume mixer：提供简单的控制各个程序音量的界面
- Netspeed：显示实时网速
- Openweather：天气预报
- Topicons plus：把状态栏（tray）的图标移动到状态栏（自带神秘特效）

GNOME 的插件其实多的能上天，装那些还是取决于你自己的口味。

#### Nautilus 插件配置

除去自带的插件，我还额外装了两个插件。如果你是 typeahead 的重度使用者（就是按键盘直接跳转到相应文件名的位置），你可能会发现新版的 nautilus 阉割了这个功能，不过安装 `nautilus-typeahead` 可以恢复这个功能。另外，`nautilus-hide` 可以帮你方便的隐藏文件，即使他并不是 `.` 开头。

#### 系统备份

我目前的解决方案是 rsync，下面给出一个我从 [Arch wiki][7] 那里魔改来的备份脚本.区别基本上就是使用了更新和删除，就是如果目标位置已经有备份，他只会更新新的文件，并删除已经被删除了的文件。另外，我还把 steam 的路径添加到了黑名单，这样可以让我节省几十个G的游戏的硬盘空间。如果要还原的话，你只需要把 rsync 的参数互换一下，就以把备份文件复制回原来的位置了。

```bash
#!/bin/sh

START=$(date +%s)
rsync -aAXvu --delete /* /Your/Destination/Directory --exclude={/dev/*,/proc/*,/sys/*,/tmp/*,/run/*,/mnt/*,/media/*,/lost+found,/var/lib/pacman/sync/*,/home/towdium/.local/share/Steam/*}
FINISH=$(date +%s)
echo "total time: $(( ($FINISH-$START) / 60 )) minutes, $(( ($FINISH-$START) % 60 )) seconds" | tee /run/media/towdium/Files/manjaro/"Backup from $(date '+%A, %d %B %Y, %T')"
```

## 结

我这里说的基本能让你的 manjaro 凑合着跑起来。实际上在我用 manjaro 的第一天就感觉这可能是我目前用过最好的发行版，尽管我这里说的似乎非常不堪的样子。实际上跳过几个雷区之后我觉得这个系统已经基本稳定了，包括很多人对于 Arch 的评价也是这样，本来以为会很不稳定，因为他们的软件包非常激进，不过正因为他们只投入到最新版本的开发，反而能把这一个版本做的非常稳定（希望不是 flag）。除去我之前提到的这些配置以外，我还有一大批精力是消耗在系统主题的配置上的。尽管 manjaro 的内置主题在众多 linux 发行版中已经是很高水平了，不过我还是果断回到了熟悉的 Numix，至于各种主题的定制，如果我有时间的话，也许会考虑再发一篇？

就这样吧。Have fun with Manjaro!


[1]:https://wiki.archlinux.org/index.php/Frequently_asked_questions#Upstream_project_X_has_released_a_new_version._How_long_will_it_take_for_the_Arch_package_to_update_to_that_new_version.3F
[2]:https://github.com/circuitikz/circuitikz/issues/46
[3]:https://wiki.manjaro.org/index.php?title=Configure_NVIDIA_(non-free)_settings_and_load_them_on_Startup
[4]:http://tieba.baidu.com/p/4125987751
[5]:https://wiki.archlinux.org/index.php/Fcitx_(%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87)
[6]:https://wiki.archlinux.org/index.php/Java_Runtime_Environment_fonts
[7]:https://wiki.archlinux.org/index.php/full_system_backup_with_rsync
