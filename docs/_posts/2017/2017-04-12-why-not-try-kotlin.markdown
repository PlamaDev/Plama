---
layout: "post"
title: "为什么不试试 Kotlin 呢"
date: "2017-04-12 21:56"
author: Author
catalog: false
tags:
  - OOP
  - Kotlin
---

你可能我还记得我在前几天把 OOP 批判了一番，不过我今天发现 Kotlin （还有 Scala）已经很好的解决了这个问题。说起来我曾经是考虑学 Scala 的，但是因为实在复杂，我看了几十页文档就基本弃坑了，不过 Kotlin 相比而言就好得多。一方面他在整体设计上都很逼近 Scala 的尿性，另一方面他的上手难度要低不少。今天大致把文档读了一下，总长度也就几十页，基本上就可以考虑上手了。他的另外一个优势是它可以认为是 JetBrains 的亲儿子，按照他们一贯的质量而言，你大概能明白稍微碰一下基本是不吃亏的。

说了这么多废话，不如说说看他们怎们的解决方案。我们先来看看我们之前的问题:

1. 在知道类名的情况下，如何构造出这个类的对象
2. 静态方法是否可以有接口
3. 构造函数是否可以有接口

要解决第一个问题，只要满足 2 和 3 的任意一个就可以，Kotlin 和 Scala 的解决方法是 2。在这一点上他们显然要比 java 走的更远一点。我们之前说 java 已经是相当严格的面向对象语言了，这样说的话，不如说 Kotlin 和 Scala 叫做“激进的面向对象语言”更为合适（笑）。他们是不允许静态函数存在的，他们对于静态函数的解决方法是这样的：每个类可以绑定一个静态对象，通过类名可以直接调用这个对象的方法。而这个对象当然是可以有接口的，而这个接口会直接绑定到原本的类上。这也就变相的实现了静态函数的接口（当然他们本质上不是静态函数）。稍微贴一段代码吧：

```kotlin
import kotlin.reflect.KClass
import kotlin.reflect.full.companionObjectInstance

fun main(args: Array<String>) {
    check(myClass = MyClass, instance = MyClass())
}

fun check(myClass: Any, instance: Any) {

    fun chkIs(name: String, obj: Any, toCheck: KClass<out Any>) =
            println("$name ${if (toCheck.isInstance(obj)) "" else "!"}is" +
                    " ${toCheck.simpleName}")

    chkIs("MyClass", myClass, MyInterfaceA::class)
    chkIs("MyClass", myClass, MyInterfaceB::class)
    chkIs("Instance", instance, MyInterfaceA::class)
    chkIs("Instance", instance, MyInterfaceB::class)

    println()

    val chk = MyClass == Class.forName("MyClass").kotlin.companionObjectInstance
    println("Class name reference ${if (chk) "==" else "!="} companion object.")
}

class MyClass : MyInterfaceB {
    companion object : MyInterfaceA {
        override fun myFuncA(a: String): Boolean = a == "Hello, world!"
    }

    override fun myFuncB(a: String): Boolean = a != "Hello, world!"
}

interface MyInterfaceA {
    fun myFuncA(a: String): Boolean
}

interface MyInterfaceB {
    fun myFuncB(a: String): Boolean
}
```

执行结果是:

```
MyClass is MyInterfaceA.
MyClass !is MyInterfaceB.
Instance !is MyInterfaceA.
Instance is MyInterfaceB.

Class name reference == companion object.
```

对于这个级别的实现，我个人已经很满意了。我现在发现我讨厌的其实并不是反射本身，而是用反射来修补语言漏洞的行为（说的就是你 java）。不过话说回来我再有意见基本上也就是我的自娱自乐吧。Kotlin 要想撼动 java 的地位，可能也是希望渺茫，但是单从我这两天的使用来看，他在设计上的确是可圈可点的。

Why not give it a try?
