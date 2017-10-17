---
layout: post
title: Just Enough Characters —— 让你的电脑爆炸
date: '2017-04-16 18:01'
author: Author
catalog: true
tags:
  - JECh
  - Java
---

## 扯两句

可能有小伙伴又要吐槽了：“你别整天想搞个大新闻”。实际上是这样的，在几天前和 cpw 的对话中，cpw 把我怒斥了一番，原话是这样的：

> that is exceptionally unwise, even today. I would be astonished that the game doesn't completely explode on you. It certainly won't be possible in the new setup, for a global transform. You could target specific chat methods to transform, of course.

大致意思就是：“妈的智障，都21世纪了还有人干这种傻逼事。赶紧给我滚回去手动兼容。你的游戏还没爆炸真是让我震惊！”然后更多的内容是：在将来的版本全局替换可能被移除。这也就意味着，以后你们可能永远不会有现在这样通用兼容的版本，in other words, you might not get such version that could make you game explode any more. （原谅我放了个洋屁）这篇我打算扯一扯以后可能的转型方案，以及我目前使用的各类算法，潜在的收集一些反馈。

## 计划

如上所说，我目前的操作方式不仅是不建议，而且以后可能就不支持了（不过可能是很久以后的事了）。所以结果就是，我大概要尽快转移到一个新的工作模式了。在 cpw 怒斥 Aroma1997 的对话里是这么说的：

>  One of the critical problems with coremods today is that people are doing laissez faire modification of every single class, even if it's completely unnecessary, causing serious performance issues. If you cannot specify what you want to change a priori, then you probably need to rethink your need.

我必须承认这句话在大部分情况下是有道理的。但是全局替换还是有一些应用场景，比如说我现在的情况，比如说某些提供跨版本兼容的 mod。当然从 forge 的角度而言这自然是无可非议的。你们可能知道 cpw 对 coremods 的意见是非常大的（即使 forge 本身也是 coremod），如你们所见（或者不见），CodeChickenCore 和 InventoryTweaks 等几个大佬在社区很大程度上是毒瘤的存在。如果你在开发过程中莫名执行到了奇怪的代码块，或者有一些奇怪的行为，又或者直接崩溃，先考虑卸载 coremods。做 NEI 兼容（我的另一个不是 coremod 的 mod）几乎是我人生中最艰难的时光。

当然你要问全局替换是不是就不可能了，我觉得未必，甚至我还不得不做一些类似的尝试。当然最粗暴的方法是直接做成核心替换 mod 进 jar 包（这都21世纪了谁还做这种事），然后赶在 forge 启动前先把 forge 黑了（笑）。这自然是说笑的，还有不少比较和谐的方法，比如偷偷把所有类名都记下来，下一次 forge 问你要列表的时候直接把所有类名都抄给他，实话说我现在确实在考虑类似的方案。大概就是把你 mods 文件夹里边的 jar 包都搜索一遍，把符合我条件的类名全抄下来。即使我的启动优先级可能不高，我这次不能直接用我下次再用还不行么？不过像这么粗暴的方法我是不是要写个 EULA？

当然其中的实现我虽然已经有大致的概念，但是还没实现。但是这并不是我主要想说的部分。因为如果这么做，仍然并不能解决他潜在的让你游戏爆炸的问题。所以我是确实有准备人工审核的。大概的实现方法就是我在客户端把 jar 包扫一遍，然后把结果发到我的机器人邮箱里。然后我就可以开始人工审核，再把审核结果发到某个页面上（目前的选择是 [towdium.github.io/class-feed.html](http://towdium.github.io/class-feed.html)）。在 mod 载入的时候，如果可以联网，就到这个页面抓取结果。这样的结果就是虽然我扫描了所有类，但是我实际上没有全局替换，而是人工审核的。另一个好处是，这样可以让你的支持库保持动态，甚至不用更新 mod，就可以接收到最新的兼容列表。这一思路能做下去的很大一部分基础就是我之前已经把类转换的相关内容全部转移到了外部的脚本驱动。

如果有幸你读到这里，你可能在想“妈的我就听他扯了这么多废话”。其实这也并不是一个空的假设，昨天我花了几个小时把相关的代码调通了，不过我还没传 github，因为找不到一个合适的地方来放。不过考虑到他们都很短，我就暂且贴在这里好了，如果有人需要也可以稍作参考。

邮件发送：

```kotlin
package towdium

import java.text.SimpleDateFormat
import java.util.*

import javax.mail.Message
import javax.mail.PasswordAuthentication
import javax.mail.Session
import javax.mail.Transport
import javax.mail.internet.InternetAddress
import javax.mail.internet.MimeMessage

/**
 * Author: Towdium
 * Date:   17-4-15.
 */

fun send(title: String, content: String, to: String) {
    val from = "jec_robot@163.com"
    val pwd = "123qwe" // Don't use my robot!
    val properties = Properties()
    properties.put("mail.smtp.ssl.enable", "true")
    properties.put("mail.smtp.host", "smtp.163.com")
    properties.put("mail.smtp.port", "465")
    properties.put("mail.smtp.auth", "true")
    val session = Session.getDefaultInstance(properties, object : javax.mail.Authenticator() {
        override fun getPasswordAuthentication(): PasswordAuthentication {
            return PasswordAuthentication(from, pwd)
        }
    })
    val format = SimpleDateFormat("yyyy-MMM-dd HH:mm:ss", Locale.ENGLISH)
    format.timeZone = TimeZone.getTimeZone("GMT")
    val message = MimeMessage(session)
    message.setFrom(InternetAddress(from))
    message.addRecipient(Message.RecipientType.TO, InternetAddress(to))
    message.setSubject("${format.format(Date())} $title")
    message.setText(content)
    Transport.send(message)
}

fun main(args: Array<String>) {
    try {
        send("TEST", "content", "jec_robot@163.com")
        println("Succeeded.")
    } catch (e: Exception) {
        println("Failed.")
    }
}
```

邮件接收：

```kotlin
package towdium

import java.text.ParsePosition
import java.text.SimpleDateFormat
import java.util.*
import javax.mail.Folder
import javax.mail.Session
import javax.mail.Store
import javax.mail.util.SharedByteArrayInputStream
import kotlin.collections.ArrayList

/**
 * Author: Towdium
 * Date:   17-4-15.
 */

fun main(args: Array<String>) {
    try {
        fetch().forEach(::println)
    } catch (e: Exception) {
        println("Failed.")
        e.printStackTrace()
    }
}

fun fetch(): ArrayList<Triple<String, String, String>> {
    val ret = ArrayList<Triple<String, String, String>>()
    val properties = Properties()
    properties.put("javax.mail.store.protocol", "pop3")
    properties.put("javax.mail.pop3.host", "pop.163.com")
    properties.put("javax.mail.pop3.port", "995")
    properties.put("javax.mail.pop3.ssl.enable", "true")
    val emailSession = Session.getDefaultInstance(properties)
    var store: Store? = null
    var folder: Folder? = null
    val format = SimpleDateFormat("yyyy-MMM-dd HH:mm:ss", Locale.ENGLISH)
    try {
        store = emailSession.getStore("pop3s")
        store?.connect("pop.163.com", "jec_robot@163.com", "123qwe")
        folder = store?.getFolder("INBOX")
        folder?.open(Folder.READ_ONLY)
        folder?.messages?.filter {
            format.parse(it.subject.substringBeforeLast(" "), ParsePosition(0)) != null
        }?.forEach {
            val content = it.content
            val subject = it.subject
            if (content != null && subject != null && content is String)
                ret.add(Triple(subject.substringBeforeLast(" "), subject.substringAfterLast(" "), content))
            if (content != null && subject != null && content is SharedByteArrayInputStream)
                ret.add(Triple(subject.substringBeforeLast(" "), subject.substringAfterLast(" "), String(content.readBytes())))
        }
    } catch (e: Exception) {
        throw RuntimeException("Fetch failed.", e)
    } finally {
        folder?.close(false)
        store?.close()
    }
    return ret
}
```

网页截取：

```kotlin
fun main(args: Array<String>) {
    val url = URL("http://127.0.0.1:4000/class-feed.html")
    val str = url.openStream().reader().use { it.readText() }
    str.replace("\n", "").replace("<br>", "\n").replace("<[^>]+>".toRegex(), "")
            .split('\n').filter(String::isNotEmpty).forEach(::println)
}
```

大部分邮件收发是基于 JavaMail 的，像我这种工科生，造轮子什么的还是算了，能用就好。唯一的代价是程序的大小可能要莫名增加好几百K。我平时是不会贴这么大块的代码的，不过如果你去查 JavaMail 的实例，他给你的内容可能要比我长上好几倍，所以就暂且贴在这儿吧。

你可能注意到了我这里写的他喵的不是 java。如果你有看我上一篇文的话，是的，我稍微有点沉迷 Kotlin。我正在考虑用 Kotlin 重写我的 mod，这有可能导致他除了要增加几百 K 的 JavaMail 以外，还要再添加一个 3MB 的依赖。你知道我对程序尺寸的要求是很苛刻的（作为电气狗），但是我这次的确在考虑要不要任性一把。

这差不多就是接下来半年的计划了，简而言之就是从全局替换转换到全局搜索-提交结果-人工审核-动态发布。这样我的工作量也许要增加一些，不过应该还在可以控制的范围。毕竟能稍微减轻一点游戏爆炸的风险也是好的。我能理解 cpw 对于性能的执着，但是实话说，我在启动上花的时间要不了5秒，所有缓存生成的时间也要不了5秒，所有缓存不超过 100MB（典型值在10MB以下），我觉得比起他的功能而言，这些性能消耗完全是可以忍受的，它可以允许你在所有涉及文本匹配的地方支持拼音搜索，而且他的匹配速度几乎能达到英文的同一个数量级。话说了这么多，我等屁民还是跟着 forge 走吧，有空说废话不如多修几个 bug。

## 算法

说起来我确实也应该把相关的算法稍微说一说了，不然显得这破站一点干货都没有，这样也不好。虽然我这里用的方式毫无槽点，甚至有点脑残，不过我还是稍微写一写，省得我之后弃坑了没人能接坑。这里主要说两个，一个是字符串匹配的算法，另一个是缓存的结构。

#### 字符串匹配

这个算法也是好几个月前写的了，现在看起来有点粗鄙，尤其是长字符串，性能没法看，我就将就解释一下，也许过几个礼拜我就把他换掉了。

第一步解析中文字符串。比如长这个字就会解析为 `{"zhang", "zh", "z", "chang", "ch", "c"}` 而 ascii 其他字符仍然会保持不变，直接解析为整形。然后再把整个字符串所有字的表示装到一个数组里。

第二步文本匹配。基本思路就是遍历所有组合，遇到不匹配的就跳出当前循环。当然我这里为了避免构造太多的字符串，实际上是用了一个光标在字符串内移动实现的。比如说他匹配了 `"zhang"`，光标就会后移5位，再匹配下一个文本。如果光标已经移动到字符串末尾，则匹配成功，如果某个文字没有能匹配的项目，就把光标退回到上个位置，重新匹配下一个组合。

我目前的感觉是他的复杂度基本上已经余地不大了，但是我可以做的是提前过滤掉很多无用项，或者在执行顺序上下功夫，让不符合的情况更早跳出。举个例子，你可以对每个文字逐个分析，而不用一上来就把整段文字解析（对于长的句子），尤其是我们现在的情况，输入的文本常常很短，而被匹配的文本可能很长，逐个解析文本可能会节省不少时间。或者，如果不考虑中英混输，可以优先分割输入的字符串，这样每个拼音段基本就确定了，这时用输入字符串匹配原字符串就可以避免很多分支。

顺便我在电脑上执行拼音字符串匹配的效率大约是5秒一百万次，纯文本字符串匹配大约0.2秒一百万次，缓存环境下0.25秒一百万次。毕竟搞科研的不能缺数据。

#### 缓存

当然如果要提高效果，效果最好，也是最简单的方法基本就是缓存了。

> 如果缓存没有给你带来性能提升的话，要么是你的缓存算法太慢，要么是缓存错了东西。 ——智障语录

说起智障语录这个说法，我昨天企图把我的微博用户名换成“Towdium-the-Stupid”，不过微博最近不让换昵称，所以就算了。

我之前在 JECh 的 [开发日志][1] 上说到过 JEI 的实现，用的是前缀树，它把一个字符串用空格分隔开，分成子字符串，然后把每个子字符串作为一个树状缓存的索引。搜索时直接通过索引调去结果////但是你也知道，中文里完全没有单词这个说法，而且汉语拼音对字段的划分都很有讲究。打个比方“长”的拼音是“chang”，但是“ng”这样的字符串并不应该匹配这个拼音。所以你知道这个设计在拼音方案上就非常蹩脚了。

我的替代方案仍然是一个近似的前缀树，但是我用搜索的字符串作为索引，不填充所有匹配项而是选择动态生成。比如说我搜索了“测试”匹配“zhizhang”，缓存的结构大概像这样：

![tree demo 1][2]

因为大部分情况这个字符串是手动打进去的，所以首先会搜索‘Z’，发现没有缓存项，就开始执行匹配，发现不能匹配，就把结果装入缓存。这时，就算你再搜索‘ZH’，甚至是‘ZHI...’，他都不会再进行匹配，因为它可以看到‘Z’本身就没有匹配通过，就跟本不用搜索更多内容了。但是如果我们搜索的是“智能”，那么他的状况基本是这样的：

![tree demo 1][3]

他会顺着树杈一直匹配下去，直到出现无法匹配的情况。平均下来，树杈每增长一层，缓存数量就能减少一半，尤其是在mc的场景下，可供搜索的词条范围基本是确定的，而且玩家的习惯也比较稳定，所以理想情况的话，他的内存消耗应该是可以控制在一个稳定的范围内的。

但是你可能发现了，在第一次搜索的时候，他的性能开销比不进行缓存的情况更大了。尤其像 JEI 这种，一瞬间就要进行成吨的运算。所以我的方法是，在玩家还没有进入游戏的时候，就先比较 JEI 的所有词条记录，手动把缓存树的前两层构造好。这个过程发生在 mc 载入时，一般在几秒钟左右。这时我们可以把目前的缓存备份下来，之后还会用到。另外一个问题是，这个缓存随着时间的增长，会逐渐膨胀，尤其是在搜索的字符串很多样的时候。所以我的解决办法是，当他膨胀到一定的尺寸的时候，就把它初始化为一开始的备份，这样我们清除了一些零碎的内容，而且基本不影响性能。这个步骤要做的所有事情就是把原来的某个分支复制一份。

当然说了这么多，如果以后有可能把字符串算法的耗时减少到目前的四分之一的话，我说不定就会把这个缓存结构给拿掉了。（妈的智障）

## 结

恩今天花了半天把这篇玩意儿码完了，也许作为开发日志？或者学习笔记？或者别的什么，这倒也没那么重要。如果有幸你读到这儿，我只能说：Thanks for your time and see you next time. 接下来一个月我就要面临大学最重要的一次期末考了，大概会失踪一段时间。

共勉。

[1]:https://github.com/Towdium/JustEnoughCharacters/issues/4
[2]:/img/posts/just-enough-characters-help-you-game-explode_1.png
[3]:/img/posts/just-enough-characters-help-you-game-explode_2.png
