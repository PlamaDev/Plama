---
layout: "post"
title: "java 中的 lambda 与函数式编程初探（下）"
date: "2016-08-31 15:28"
author: Author
catalog: true
tags:
  - java
---

## lambda 语法

在看完上一篇的一大堆铺垫之后，我们终于要进入正题了。不过话说回来，我们还是要明确这一个概念：lambda 只是对应函数对象（再退一步说，匿名内部类）的语法糖而已。是的，仅此而已。

#### lambda 表达式

重新看一下我们之前写的分数类，可以看到其中代码的效率是非常低的（是代码的写作效率，不是运行效率），或者说，我们其实为了语法需要，废了很多的口舌在说废话。这时候就轮到我们的lambda表达式上场了。跟前几次一样，我们将之前分数类的代码用 lambda 重新写一下：

```java
public class Fraction {
    int numerator, denominator;

    public void plus(Fraction f) {
        operate(f, (a, b) -> a+b);
    }

    public void minus(Fraction f) {
        operate(f, (a, b) -> a-b);
    }

    public void operate(Fraction f, BiFunction<Integer, Integer, Integer> func) {
        denominator = denominator * f.denominator;
        numerator = func.apply(denominator * f.numerator, numerator * f.denominator);
        // ... other operations
    }
}
```

这里的代码完全和上一段等价。这里你就可以看到，函数式编程 + lambda 语法真正的提供了便捷。这里的 lambda 表达式 `(a, b) -> a+b` 完全等价于原先匿名内部类定义。我们简略的说一下它的语法。他有两部分，箭号左侧是函数参数，右侧是函数体。这里我们给出几个例子：

```java
a -> a + 1;   // 接收参数 a，返回 a+1
(a) -> a + 1;   // 单参数可以加括号，也可以不加
(a, b) -> a + b;    // 多个参数必须加括号，返回 a+b 的值

a -> {
    System.out.println("I'm writing a lambda");
    return a;
}   // 多行函数体要用大括号表示，返回值要用 return 标明

(a, b) -> {
    System.out.println("Multiple parameter and multiple lines");
    return a + b;
}
```

可以看到函数参数有两种形式：单参数可以不带括号，多参数需要带括号。函数体也有两种形式：单行函数体和多行函数体。单行函数体的执行结果直接作为返回值返回，不需要写return。多行函数体和普通的函数无异，返回值用return标明。

#### 变量捕获

lambda 还有另外一个非常有用的特性，就是变量捕获。他允许你使用当前语境下的局部变量以及当前对象的成员变量。这其实是一个继承自内部类的特性。通过这个操作我们可以轻易地操作表达式语境内的变量。下边给一个例子,我们通过 foreach，把 `List<Integer> source` 里边的所有内容添加到 `List<Integer> dest` 里，代码如下：

```java
List<Integer> dest = new Arraylist<>();
source.foreach(i -> dest.add(i));
```

这一操作实际上相当于在内部类里引用了外部语境的变量，相关的操作方式应该在学习 java 基础的时候就已经接触过了，我这里就不多说了。唯一的一个关注点 effective final，我会在附言部分说到。

## 附言——各种细节与小技巧

#### final 相关

出于安全考虑，如果你要在 lambda 表达式里边捕获局部变量，有一个问题是你务必要注意的：被捕获的变量需要是 effective final。这是 java 制造出的说法（其实有点纠结），也就是说他可以不是 final，但是加上 final 之后应当仍然能编译通过。这个特性在各篇文章里已经被轮流批判过了，认为他极大的阻碍了 lambda 的拓展性。其实不然，我这里就从另一个角度说一下他的设计意图，以及应对方法。

我们已经知道了 lambda 其实就是局部匿名内部类的实例，java 出于统一性和兼容性的考虑，并没有为 lambda 添加新的结构，于是 lambda 表达式也自然继承了局部匿名内部类的这个特性。这个现象的根本来自于局部变量与内部类实例的生命周期的不同，具体内容可以参考 [这篇][3]。java 在编译时将捕获的变量隐式传入了内部类对象。这就意味着对于内部类而言，这实际上只是一个传入的对象，并不是一个外部变量的引用。所以说假如外部变量发生了改变，内部类所持有的这个对象并不会相应的变化，就会带来同步的问题。这里我们想到 foreach 的内部迭代在相当程度上就是为了解决多线程遍历问题，这就意味着 lambda 对象应该在多线程环境下正确运行，而 lambda 内部持有的对象不可以同步改变，所以结果显而易见：外部变量需要是一个定值，才能保证我们这个 “捕获” 的语法糖能正确使用。同时，为了代码的美观，java允许你在这种情况下省略被捕获变量的 final 关键字，但是他本身应该是 final 的，这就是所谓的 effective final。

说了这么多，你大概可以明白，下面这段代码是编译不过的：

```java
int i = 0;
List<Integer> l = new ArrayList<>();
l.forEach(a -> {
    i++;    // 捕获的变量应为final，不允许赋值
});
```

这样虽然因为 final 的问题不能编译，但是我们其实可以通过一个简单的包装来曲线救国，这里因为有多个类，我把代码完整的写一下：

```java
public class Demo{
    public void main(String[] args) {
      IntegerWrapper i = new IntegerWrapper();
      List<Integer> l = new ArrayList<>();
      l.forEach(a -> {
          i.value++;    // 这里捕获的i是final，但是我们可以修改它的成员变量
      });
    }

    static class IntegerWrapper{
        public int value = 0;
    }
}
```

注意这里用原生的 Integer 类是不可以的，下面代码同样无法编译通过：

```java
final Integer i = 0;    // 这里的final可加可不加，没有实质影响
List<Integer> l = new ArrayList<>();
l.forEach(a -> {
    i++;    // java原生的包装类Integer的值是final，同样不允许赋值
});
```

通过这一层包装，内部类得到的其实是一个包装类的对象，或者说就是一个对象的引用。如果你熟悉 c 的话，可以发现这和常用的值传递／引用传递非常相似，只是 java 对于基础类型全部使用值传递，对于引用类型全部使用引用传递，我们通过这一层包装，就可以获得一种更灵活的操作方法。通用的解决方案，就是一个单独的泛型包装类，这也是我目前比较常用的方法。上面一段代码可以这样改写：

```java
public class Demo{
    public void main(String[] args) {
      Wrapper<Integer> i = new Wrapper<>(0);
      List<Integer> l = new ArrayList<>();
      l.forEach(a -> {
          i.value++;    // 这里捕获的i是final，但是我们可以修改它的成员变量
      });
    }

    static class Wrapper<T>{
        public T value = 0;

        public Wrapper<T>(T value) {
            this.value = value;
        }
    }
}
```

这样几乎就可以解决 effective final 带来的所有问题了。至于包装类的效率，考虑到这里需要包装的对象基本上只有需要共用的几个，应该不会有可见的区别。

#### lambda 效率

这里我只粗略的说，不给出具体的数据。我之前读过一篇文章，有一些相关的数据，结论是 for 循环和 foreach 循环效率基本相当，foreach() 遍历稍慢（不是数量级上的慢）。在了解了整个结构之后你可能就会明白，for 和 foreach 只是流程上的循环，不牵涉到 function call，而从最好的情况来看 foreach 函数每次迭代都要多进行一次 function call。而对于一些简单的操作，牵涉到参数的入栈和出栈，现场的还原，高频率的 function call 的耗时在数据层面上是可见的，但是对容量很大的容器，foreach 在并行处理上的优化潜力仍然不可小觑。另外，我原本以为 lambda 表达式造成的函数对象的新建和销毁也会有性能消耗，实际上并不是这样，对于 lambda 表达式，函数对象的构造只进行一次，也就是在调用 foreach 函数的时候，所以并不会带来对象构造和销毁的负担。

#### 函数引用

这也是 java8 带来的新的语法糖，通过函数引用，我们可以把一个普通的函数转化成函数对象：

```java
public class Demo {
    public void main(String[] args) {
        Runnable r1 = Demo::funcStatic;   // 静态函数
        Runnable r2 = new Demo()::funcNonStatic;    // 非静态函数
    }

    public static void funcStatic() {
        System.out.println("This is a static function");
    }

    public void funcNonStatic() {
        System.out.println("This is a non-static function");
    }
}
```

语法上很简单，把调用函数的点号换成双冒号，并且省略掉参数的括号，就是函数引用的表达式了。相同的，静态方法可以通过对象或者类名访问，动态方法必须通过对象访问。很多人会把函数应用作为 lambda 的另一种语法，这样说也是不无道理。因为函数引用在本质上也是函数对象的语法糖：在上面这一段里，以下两个表达式是等价的：

```java
Runnable r3 = Demo::funcStatic;
Runnable r4 = () -> Demo.funcStatic();
```

#### 内部迭代与外部迭代

从面向对象的角度来说，一个 Collection 理应提供一个用于迭代的方法，就像我们之前提到的 Collection 的 foreach。但是这个方法的特殊之处在于，参数是一个 “操作” 而不是一个值，但是很多早期的语言对于函数式编程的应用并不是很普及，于是，我们实际上很广泛的在使用一种替代的方法，也就是外部迭代。在外部迭代的环境下，程序员需要构建整个迭代的环境，简单的如迭代器+for循环，对于很大的集合可能就需要使用多线程进行，这些所有迭代方式都是使用者确定的，Collection 本身只负责提供一个迭代器。这时你可能就发现问题了，在这个问题上，类库的封装程度比较低，相当一部分操作是暴露给开发者的。这就是外部迭代的问题所在，而且一个类库完全可以提供一种通用而高效的迭代方案（当然迭代器已经很不错了，但是内部迭代的优化空间会更大）。于是，随着编程语言的发展，我们就有了内部迭代。类库接受一个操作（或者说是操作者的对象，lambda 只是一种特殊的写法），应用于 Collection 内部的所有对象，所有迭代操作都在类库的内部完成。

正如前文所提到的，如果我们只是需要对于一个 Collection 内部的所有对象进行同一种操作，这显然应该是最方便，也理应是效率最高的方式。但是问题在于整个迭代过程现在不归开发者控制了，所以对于一些复杂的操作可能就并没有那么方便了。至于这种情况，我们会在下一段 “流” 里说到，但是使用迭代器带来的灵活性几乎是无可撼动的。

所以我们对内部迭代和外部迭代做一个归纳：

- 内部迭代优势：
  - 语法简洁，符合面向对象思路
  - 类库有很大的优化空间
- 外部迭代优势：
  - 语法灵活，可控性强
  - 作为不同 Collection 无可替代的交互接口

所以我们预见到，随着函数式编程的深入人心，内部迭代在大部分情景下将会成为主流，但是迭代器作为重要的算法基础，在某些特殊场景下仍然无可替代。

#### 流

这一部分也是 java8 新增的重要特性。实际上 java8 为了使原生类库能够更好的适应函数式编程的思路，对标准库进行了大规模的扩容，更多内容参见 [这个站点][4] 的函数式编程相关文章，我这里只是简介一下，点到为止。

我们可以看到 java 标准库在 Collection 这一部分对于函数式遍历，只提供了 foreach 函数，这在很多情况下并不能满足我们的需要，所以 java 给我们提供了 Stream 这个类，主要就是用于通过函数式思路控制复杂的集合遍历，而所有的 Collection 都可以和 Stream 无缝转化（而且这一步几乎没有耗时）。这里必须要指出的一点是，Stream 本质上是一个用于迭代的临时结构，有点像一个函数式加强版的迭代器，勿当作容器使用。Stream 提供了相当多的方法，我这里只给出一个应用，从小到大打印一个容器里大于5的数：

```java
list.stream().filter(i -> i > 5).sorted((a, b) -> a - b)
.forEachOrdered(System.out::println)
```

这里 filter 方法对流进行过滤，sorted 方法进行排序，forEachOrdered 进行遍历。（这段代码我没跑过，不过应该是这么写没错）唯一要注意的是我们之前说了 foreach 方法本身是不考虑顺序的，所以如果要按顺序遍历的话，需要用 forEachOrdered。通过流进行遍历使得我们代码的可读性明显增强了，而且我们可以连续调用，非常方便。

## 写在最后

这篇文章我原本打算很轻描淡写的装个逼走人，后来想到我之前读相关的文章完全是一脸懵逼，于是打算稍微把相关的东西讲清楚，争取让这方面零基础的小白也能明白我在说什么，于是就有点一发不可收拾的意思了，以至于我不得不分p来控制篇幅。结果就是我这里说的内容给人一种事无巨细的感觉，这其实就是我写这种入门指引的风格。我尽量把相关的点都讲到，内容上仅仅是点到为止。如果你真的有兴趣，我提供的内容足够帮助你在百度上找到一些更细节的文章了，我觉得这样就很好。

不得不承认码字这种事是十分辛苦的。我之前看知乎上有人分享日常心得，说他坚持 “日写千言” 超过一年，深感其中的好处，这句话我同意一半。写一些简单的技术文章对于我这种小白来说确实有很大的帮助，一知半解是没法把事情说清楚的，所以经常是我原本只是想分享一下近期获得的小技巧，结果是研究了半天，读了一大通文档和源码。这篇文章的一大半内容是我坐火车的路上写的，剩下填充例程，整理文本结构，核实内容几乎花了我三个晚上。所以对我来说，重要的不是写了多少东西，而是写的这个过程能帮助我理清思路。“吾尝终日而思矣，不如须臾之所学也”，我对码字并没有什么特别的兴趣，所以 “日写千言” 这种事我是肯定不会做的，我最大的希望就是把我要说的东西说清楚又不要过于累赘。如果能帮到各位自然是好的，同时我也很担心我里边是不是有一些欠考究的会误导到各位（笑）。所以呢，如果你发现有什么错误或者不严谨的地方，欢迎跟我提出来，我会尽早订正。

[3]: http://blog.csdn.net/edisonlg/article/details/7183373
[4]: http://it.deepinmind.com/index.html
