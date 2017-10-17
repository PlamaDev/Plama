---
layout: post
title:  "浅析 Java 中 enum 枚举与 C 的异同以及使用方法"
date:   2016-01-30
author: Author
catalog:    true
tags: [java, c]
---

## enum 的前世今生 —— C 与 Java

相信有不少小伙伴儿们刚开始接触 enum （枚举）的时候和笔者的想法是一样的：
“我知道 Java 里边结构主要就是 类，方法，变量，这个枚举是个什么玩意？”
这个问题现在解决可能还为时过早，我们不妨先看一段 C 的代码，
来看看 enum 在 C 中是怎样的存在。没有 C 基础的小伙伴儿也别激动，
我尽量把它写的简单一些，没有基础应该也能看懂，或者直接读下一节也不会影响整体内容的理解。
注意，作为一个面向过程的语言，
C 语言是没有类的，取而代之的是 structure （结构体），
你可以把它理解成只有变量没有方法的类,我们在后边会用到。

```c
#include <stdio.h>

int main(){
    /*
    * 新建 enum 类型的变量 myLeverState 表示拉杆状态
    * 同时，myLeverState 受前边的内容限制，
    * 需要表示 LeverState，且内容必须是 ON 或 OFF 之一
    */
    enum LeverState{ OFF, ON } myLeverState;
    myLeverState = ON;  /* 将变量赋值为 ON */
    printf("%d", myLeverState);
    /* 输出结果：1 */
    /* C语言把 ON 和 OFF 翻译成了整数，按照顺序，OFF为0，ON为1 */
}
```

在这一段代码里，我们可以相当清楚的接收到 C 语言设计的时候的思想：

> enum 是一种特殊类型的变量，它的值是受限定的。

但是这句话未必经得起考究。很明显，变量声明时，声明 enum 的方式和其它变量并不一致,
这也就意味着，enum在本质上可能未必这么简单：

```c
int i;  /* 整型声明 */
float f;  /* 浮点型声明 */
enum Type{ A, B, C } e;  /* 枚举型声明 */
```

马脚很明显，我就想问你这里多出来的一段 `Type{ A, B, C }` 是几个意思？
鉴于C语言中大括号的使用并不多，
我们很快就想到了 structure 的定义。如果我们要刻意将 structure 的声明逼近 enum，
代码也可以这样写：

```c
#include <stdio.h>

int main(){
    /* 新建 LeverState 类型的结构体，新建一个实例为 leverState */
	struct LeverState { int OFF = 0, ON = 1; } leverState;
	int myLeverState = leverState.ON;   /* 给整型变量赋值为 ON */
	printf("%d", myLeverState);   /* 结果仍是1 */
}
```

对比一下，上下两段代码完全就是激似好么！上边的代码可能和我们平时的习惯有所出入，
但是完全是可以编译执行的。所以，我们不如这样说：

> C 语言中 enum 在表层上是一种特殊的变量类型，在底层的实现可能更接近 structure

我并没有研究过 C 语言的底层，但是这一推测至少是有理有据的。
同时，他们之间还存在着以下的区别：

* enum 的内容是不可更改的，而 structure 里的变量可以随时更改
* enum 中的变量类型是一致的（从第一段代码我们可以看出所有的值其实都被解释为了整型数字），
而 structure 里可以存在不同类型的变量
* enum 的每个实例都只表示一种情况（一个变量的值），而 structure 的实例包含所有变量

所以，虽然 enum 和 structure 是非常相似的，他们的用途却完全不同。
但是这一发现还请暂时记住，在 java 中，相似的情况也将会出现。
不过此时 structure 的角色将由 java 中的增强版 —— 类 来代替。

## enum 在 java 中的用法

我们终于离开了让人蛋疼的怀旧内容，能开始说一点儿正经事了。
在开始之前，回想一下我们之前的结论：
C 语言中的 enum 与 structure 具有相当高的相似性。
那么，java 中的 enum 是否就会相应的和类具有一定的相似性呢？
答案是肯定的，而且理论上来讲，对编译器而言，enum 实际上就是 class。
所以在接下来的内容中，从 class 的角度理解 enum 的一些语法， 会带来一些帮助。
先来看一段代码吧，继续用我们的拉杆的模型：

```java
public class Lever {

    enum State{ON, OFF};    // 声明一个enum，可能出现的情况有 ON 和 OFF

    public static void main(String[] args){
        State myLeverState = State.ON;
        System.out.println(myLeverState);  // 打印结果 ON
    }
}
```

如果这一段代码还不够简洁易懂的话，不如参考下边这段代码。鉴于 java 多样的修饰符，
我们已经几乎可以完全用 class 的方式实现 enum 了，这在 C 语言里是做不到的：

```java
public class Lever {

    static class State {
        public static final State ON = new State("ON");
        public static final State OFF = new State("OFF");

        private final String name;  // enum的名字

        private State(String str){   // 构造方法
            name = str;
        }

        @Override
        public String toString(){
            return name;
        }
    }

    public static void main(String[] args){
        State myLeverState = State.ON;
        System.out.println(myLeverState);    // 打印结果仍然是 ON
    }
}
```

可以看到，我们 main 方法中的内容并没有变化。通过类 State ，
我们模拟了 enum State 的工作方式。
实际上，真正的 enum 实现和我们这里给出的代码是很相似的，
只是 java 提供了更多的方法便于我们使用。
在运行时，每个 enum 都是一个 Enum 类的子类，
从而继承了 Enum 类的方法。

从这个例子来看，我们已经可以得到一些 enum 的特点了：

* 静态且不可变更
* 类中包含自身的常量对象，即可能出现的值
* 私有构造方法，防止 enum 被拓展。

说到这里，如果你对于 enum 本质是 class 这件事还不是十分确信的话，
我们再来看一段代码：

```java
public class Lever {

    enum State{ ON, OFF }

    public static void main(String[] args){
        State myLeverState = State.ON.OFF;
        System.out.println(myLeverState); // 打印结果 OFF
    }
}
```

在这里， `State.ON.OFF` 就是通过成员变量调用类的常量，执行结果没有任何问题。
尽管 IDE 在这里给我报了一个警告，问我调用这个变量为什么要绕这个大圈子（笑）。

## enum 的应用 —— 与类的常量的比较

我们到 java 源代码里随便找点东西看看，嗯，就你了 Color （在 java.awt 下）。
一打开我们就发现满眼都是常量,一下子占了两百行。我摘抄一段源码感受一下：

```java
public class Color implements Paint, java.io.Serializable {
    public final static Color white = new Color(255, 255, 255);
    public final static Color WHITE = white;
    public final static Color lightGray = new Color(192, 192, 192);
    public final static Color LIGHT_GRAY = lightGray;
    public final static Color gray = new Color(128, 128, 128);
    public final static Color GRAY = gray;
    // 省略 n 多行
```

虽说也没什么不对的，不过总觉得不太好。如果我们用枚举来做呢？

```java
public class Color {
    enum Colors{
        RED, BLUE, YELLOW, GREEN, LIGHT_GREY,
        DARK_GREY, GREY, BLACK, WHITE
    }
}
```

有木有觉得眼前清静了许多呢（笑）。

不过这样也不是完全解决问题的，或者说，完全不解决问题。
比如一个函数要求参数是 Color 类的，
那么 `Color.Colors.YELLOW` 这样的玩意儿是完全不管用的，
因为人家要的是 Color 类，我们给的却是 Colors 类，换句话说，是 Enum 的子类。

这个时候 enum 的特征就很明显了，他只是一个标记，本身是不装内容的。
如果我们硬要用 enum 来做呢，也不是不可以，下面提供两个思路：

思路一：

```java
public class Color {

    enum Colors{
        RED(255, 0, 0),
        YELLO(255, 255, 0),
        BLUE(0, 0, 255),
        GREEN(0, 255, 0),
        GREY(128, 128, 128),
        LIGHT_GREY(192, 192, 192),
        DARK_GREY(64, 64, 64),
        BLACK(0, 0, 0),
        WHITE(255,255,255);

        private Color color;  // 在 enum 内部保存一个 Color 类的实例

        private Colors (int r, int g, int b){  // 新建 enum 时就将这个 Color 赋值
            color = new Color(r, g, b);
        }

        public Color toColor(){   // 需要时通过特定的函数调用
            return color;
        }
    }

    int red,green,blue;

    Color(int r, int g, int b){
        red = r;
        green = g;
        blue = b;
    }

    public static void main(String[] args){
        Color c = Color.Colors.BLUE.toColor();
    }
}
```

思路二：

```java
public class Color {

    enum Colors{
        RED, BLUE, YELLOW, GREEN, LIGHT_GREY, DARK_GREY, GREY, BLACK, WHITE;

        public Color toColor(){   // 需要时新建并返回对象
            switch (this){
                case RED: return new Color(255, 0, 0);
                case YELLOW: return new Color(255, 255, 0);
                case BLUE: return new Color(0, 0, 255);
                case GREEN: return new Color(0, 255, 0);
                case LIGHT_GREY: return new Color(192, 192, 192);
                case DARK_GREY: return new Color(64, 64, 64);
                case GREY: return new Color(128, 128, 128);
                case BLACK: return new Color(0, 0, 0);
                case WHITE: return new Color(255, 255, 255);
                default: return null;
            }
        }
    }

    int red,green,blue;

    Color(int r, int g, int b){
        red = r;
        green = g;
        blue = b;
    }

    public static void main(String[] args){
        Color c = Color.Colors.BLUE.toColor();
    }
}
```

思路三：

```java
import java.util.EnumMap;

public class Color {

    enum EnumColors{RED, BLUE, YELLOW, GREEN, LIGHT_GREY, DARK_GREY, GREY, BLACK, WHITE}

    // 建立 EnumMap 来存储 EnumColors 与 Color 的对应关系
    static EnumMap<EnumColors, Color> colors = new EnumMap<EnumColors, Color>(EnumColors.class){
        // 通过匿名内部类复写构造方法，实现内容的初始化
        {
            put(EnumColors.RED, new Color(255, 0, 0));
            put(EnumColors.YELLOW, new Color(255, 255, 0));
            put(EnumColors.BLUE, new Color(0, 0, 255));
            put(EnumColors.GREEN, new Color(0, 255, 0));
            put(EnumColors.LIGHT_GREY, new Color(192, 192, 192));
            put(EnumColors.DARK_GREY, new Color(64, 64, 64));
            put(EnumColors.GREY, new Color(128, 128, 128));
            put(EnumColors.BLACK, new Color(0, 0, 0));
            put(EnumColors.WHITE, new Color(255, 255, 255));
        }
    };

    int red,green,blue;

    Color(int r, int g, int b){
        red = r;
        green = g;
        blue = b;
    }

    public static void main(String[] args){
        // 通过 map 提取 Color 类型的对象
        Color c = Color.colors.get(EnumColors.BLUE);
    }
}
```

我必须承认这三个方法毫不简便，但是第一段我们可以看到 enum 的拓展性。
修改构造方法，重写原有方法，添加新方法都没有任何问题，
第二段我们能看到 enum 与 switch 的组合使用。
第三段是我对于 EnumMap 的强行展示（笑）希望各位能看懂（看不懂也没有任何卵子关系）。
至于他能实现怎样的功能，就看各位自行发挥了。

这样看来，要想用 enum 来替代常量，不仅在开始写的时候要多一些内容，
而且还会增加调用时候的复杂性，所以未必是个好方法。相反地， enum 的优势在于：

1. 所有值由特有的名称区分，不易混淆
2. 可用内容在声明时就已经规定，可以规避非法参数的出现
3. 丰富的的拓展性，包括 EnumMap, EnumSet 的支持，以及 iterable 等接口的支持


## enum 的应用 —— 传参与 foreach

鉴于前文中提到的一和二的优点，enum 其实可以用来做函数的参数。
举个栗子吧，比如我们现在有一个电磁炉，工作时可以有不同的模式。
如果不用 enum， 我们大概会这样写：

```java
import static java.lang.System.*;

public class Oven {
    public static final int WORK_MODE_LOW = 1;
    public static final int WORK_MODE_MID = 2;
    public static final int WORK_MODE_HIGH = 3;

    public void cook(int mode){
        out.print("Working in ");
        switch (mode){
            case WORK_MODE_LOW: out.println("LOW mode"); break;
            case WORK_MODE_MID: out.println("MID mode"); break;
            case WORK_MODE_HIGH: out.println("HIGH mode"); break;
        }
    }

    public static void main(String[] args){
        Oven oven = new Oven();
        oven.cook(WORK_MODE_MID);  // 输出 Working in MID mode
    }
}
```

上边大概不用我写注释了吧（笑）。有了 enum 之后， 我们可以这样写：

```java
import static java.lang.System.*;

public class Oven {
    enum CookMode{LOW, MID, HIGH}

    public void cook(CookMode mode){
        out.print("Working in " + mode + " mode!");
    }

    public static void main(String[] args){
        Oven oven = new Oven();
        oven.cook(CookMode.MID);  // 输出 Working in MID mode!
    }
}
```

有木有觉得超爽的说！

另外，Enum 的实例具有 values() 方法，这个方法会返回 enum 所有取值的集合。
说道这里有没有想到什么？集合是可以丢到 foreach 里边循环的！
所以，我们还可以这样玩儿：

```java
import static java.lang.System.*;

public class Oven {
    enum CookMode{LOW, MID, HIGH}

    public void cook(CookMode mode){
        out.println("Working in " + mode + " mode!");
    }

    public static void main(String[] args){
        Oven oven = new Oven();
        for(CookMode mode : CookMode.values()){
            oven.cook(mode);
        }
    }
}
/**
 * 输出结果：
 * Working in LOW mode!
 * Working in MID mode!
 * Working in HIGH mode!
 */

```

看到这儿，有没有突然发现 enum 还是有点意思的。
本人才疏学浅，就不继续献丑了。

----

参考文献：
[匿名内部类构造函数](http://blog.csdn.net/brockliu1010/article/details/12440019)
[Java enum的用法详解 by rhino](http://www.cnblogs.com/happyPawpaw/archive/2013/04/09/3009553.html)
