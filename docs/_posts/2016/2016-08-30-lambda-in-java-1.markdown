---
layout: "post"
title: "java 中的 lambda 与函数式编程初探（上）"
date: "2016-08-30 00:22"
author: Author
catalog: true
tags:
  - java
---

今天来更一更最近对java的学习内容。主要是关于java的函数式编程，文中会说到lambda表达式的用法，并提供一些事例和实际问题中的使用技巧。

## lambda 何物

lambda实际上在各个语言中广泛使用的一种语法结构，被当做匿名函数来使用。基于java的内存机制，lambda在绝大部分情况下会被处理成一种类似于内部类对象的结构，具体的内容我们会在稍后说到。利用这个特性，我们可以在代码的不同位置便捷的进行函数的定义，并且可以将其作为对象进行传递。这都是相当方便的特性。

#### java 中函数对象的实现

查一下文档，我们可以在 java.utl.function 这个包下面发现一些事先定义好的函数式接口。比如说最常用的 `Function<T, R>`，这个接口规定了apply的方法，接受一个 `T`，返回一个 `R`。所以我们只需要这样写，就可以定义一个函数对象 func，并且在代码中通过这个对象来调用相应的函数：

```java
Function<Float, Float> func = new Function<Float, Float>() {
    @Override
    public Float apply(Float x) {
        return x+1;
    }
};
System.out.println(func.apply(1)); // 调用函数，打印结果为2
```

除此之外，还有我们比较熟悉的 Runnable 接口，或者是其他的自定义函数式接口，都是可以使用的，用法也是大同小异。这里我们可以看到 java 的函数是一系列特殊的接口，在大多数情况下，我们通过这些接口的实现类的对象（即函数对象）来执行需要的方法。java 给这类特殊的接口定义了一个 annotation，`@FunctionalInterface`，来明确的表明这是一个函数式接口。这和 c++ 里所谓的 “函数对象” 的相似度是很高的，只不过在c++里我们习惯用运算符重载（一类特殊的函数）来实现。至于这个标记 `@FunctionalInterface`，官方文档说了这个标记并不是必需的，所有符合条件的接口都会被编译器当作函数接口来处理。他需要满足以下条件：

- 必须是一个接口类型，不可以是 annotation，类，或者是枚举类型；
- 除去 Object 的 public 方法之外，只能有一个抽象方法（当然 Object 的 public 方法说了也几乎等于废话）。

至于第二点，只允许一个抽象方法，这也就意味着你可以用 java8 的新特性 “default 方法”，来给接口定义一些额外的方法，更多内容可以参考 [这里][1]。

这里我只给出了一些概念的解释，更具体的内容可以参见 [这篇][2]。

#### lambda 表达式与函数对象

至于 lambda 又是什么，其实就是一种语法糖，专门用于简化函数对象的语法。比如说上面那一段代码，我用 lambda 改写一下，大概就是这样：

```java
Function<Float, Float> func = x -> x+1;
System.out.println(func.apply(1));
```

有没有很爽呢？至于具体的语法我们会在下面说到。实际上规律也很简单，你可以发现他是一个 “... -> ...” 的形式，箭号左边对应着参数声明 `Float x`，右边对应着那一行有用的代码 `return x+1`，而原来的所有其他内容，java 都通过类型推测帮你搞定了。其实也并没有特别难理解吧，不过是语法糖而已。

## lambda 有什么用

在上面的段落，我们已经给出了java中函数的封装形式。即使不使用lambda,我们也已经可以通过这些特殊的接口来实现大部分函数式编程的特性。本质上而言，lambda 实际上只是对应函数对象操作的语法糖而已，所以 lambda 的用途其实也就等同于函数式编程的用途。说了函数对象的语法之后，我们抛开 lambda 语法，先来说一说函数式编程的一些应用情景。说白了就是把函数作为参数传递到另一个函数中，或者是返回一个函数对象的操作。

#### 回调函数

因为我这篇文章是比较新手向的，所以我这里粗略的解释一下回调函数的含义。举个栗子：有一天你去商场买东西，服务员告诉你你要买的东西已经卖光了。然后你就把手机号留给他，让他到货就打电话通知你。这个情况下，相当于顾客调用了售货员的“等待进货”这个函数。作为函数的执行结果，我们显然不能通过返回值来获得，因为这一动作是有明显的延迟特性的。售货员只能反馈给你“好的，我会尽快告知您”，但是没法告诉你具体的时间。所以你需要提供一个反馈的方法，也就是“给我打电话”。这个方法作为一个接口，在满足条件时就会被调用，将结果反馈给调用者。这个方法也就是所谓的回调函数。这里插一段代码示意一下。

```java
public class Salesman {
    List<Runable> duty = new Arraylist<>();

    public boolean notifyWhenAvailable(Runable r) {   // 记录回调函数
        duty.add(r);
        System.out.println("I will inform you as soon as possible, sir.")
        return true;
    }

    public void onAvailable() {   // 到货时调用回调函数进行反馈
        for(Runable r : duty) {
            r.run();
        }
    }

    public void main(String[] args) {
        Salesman sm = new Salesman()  // 不要对这个变量名有什么非分之想
        sm.notifyWhenAvailable(new Runable() {  // 提供回调函数
            System.out.println("Call 110.");
        })
    }
}
```

#### foreach

foreach 是我们日常使用中很经典的函数式编程案例。当我们需要遍历一个 Collection 的时候，很多人的答案会是迭代器，像这样：

```java
Iterator<Integer> i = collection.iterator();
while(i.hasNext()) {
    System.out.println(i.next());
}
```

或者对于各种实现 Iterable 的类，java 还有这个语法的补充:

```java
for(Integer i : collection) {
  System.out.println(i);
}
```

或者对于 List 来说，还可以这样：

```java
for(int i = 0; i < list.size(); i++) {
    System.out.println(list.get(i));
}
```

这当然都可以，但是你会发现我们在这几种写法中都有一个相当固定的结构来执行这个遍历操作，我们暂且把它叫做 “框架代码”，也就是那些实际意义不大的，大家闭着眼就能默写出来的代码。如果你仔细的看过文档的话，可能会注意到 Collection 下边还有一个方法 foreach，这就是我们所说的内部迭代。我们这里举个栗子，仍然进行上文同样的操作:

```java
list.foreach(new Consumer<Integer>() {
    @Override
    public void accept(Integer integer) {
        System.out.println(integer);
    }
});
```

通过这样的调用方式，终于满足了我们对于闭包的强迫症，这就是所谓的内部迭代，整个迭代操作在类库的内部完成（更多内容参见附录），函数接受一个操作，应用于容器内的所有对象，思路上无懈可击。如你所见，这里的语法其实并不算简洁，如果我们用 lambda 改写，则是这样：

```java
list.foreach(integer -> System.out.println(integer));
```

看到这里的各位可能不禁要叫到 “Cool！”，其实通过函数引用，这一语句还可以化简为：

```java
list.foreach(System.out::println);
```

当然这和我们要说的 lambda 关系不大，不过也是 java 对于函数式编程的相当好用的语法。关于函数引用的内容，你可以在附言里找到。

#### 简化重复代码

看到这里，你对于函数式编程的情景应该有一些概念了。当你对这种思路有一些思考之后，你就会发现他可以简化相当多的重复代码。有兴趣的可以参见我正在进行重构的项目 JustEnoughCalculation, 将近三分之一的原有代码在使用函数式思路之后被化简了。不过这带来的弊端就是写代码开始变得有些消耗脑力，并且对于入门者不是十分易懂了。（真的是弊端么？）优势在于他提供了非常强的扩展性以及很高的灵活度，并且也更易于维护了。这里给出一个例子：我们写了一个分数类，提供两个方法进行分数的加减，代码如下：

```java
public class Fraction {
    int numerator, denominator;

    public void plus(Fraction f) {
        operate(f, new BiFunction<Integer, Integer, Integer>() {
            @Override
            public Integer apply(Integer a, Integer b) {
                return a + b;
            }
        });
    }

    public void minus(Fraction f) {
      operate(f, new BiFunction<Integer, Integer, Integer>() {
          @Override
          public Integer apply(Integer a, Integer b) {
              return a - b;
          }
      });
    }

    public void operate(Fraction f, BiFunction<Integer, Integer, Integer> func) {
        denominator = denominator * f.denominator;
        numerator = func.apply(denominator * f.numerator, numerator * f.denominator);
        // ... other operations
    }
}
```

这里你可以看到通用的操作都被写在了 `operate` 这个方法里，而 `plus` 和 `minus` 两个方法只需要提供两者的不同的部分（当然这里的内部类语法很繁琐，我们很快就会解决这个问题），从而可以简化相当一部分的重复内容。

> 这一篇中，我们已经简单的介绍了函数式编程的思路以及应用情景；下一篇，我们会着重于 lambda 的语法以及实际操作中的注意点。

[1]: http://blog.csdn.net/chszs/article/details/42612023
[2]: http://colobu.com/2014/10/28/secrets-of-java-8-functional-interface/
