---
layout: "post"
title: "说说面向对象的反序列化为什么让我绝望"
date: "2017-04-05 23:27"
author: Author
catalog: true
tags:
  - Java
  - C
  - OOP
---

## 前言

当我写下这个标题的时候，我突然想起了最近炒得火热的“震惊部”。我倒是没有要搞个大新闻的意思，只是最近这破事搞得我有点焦虑。说起来也不算是什么新鲜事了，就是所谓的序列化与反序列化，更深层次说，就是在只有类名的情况下如何进行对象构造。我觉得我在所有熟悉的面向对象语言里，都没法找到一个堪称典范的实现方式，或者说，面向对象本身在这个问题上就有些捉衿见肘。我这里并不想抓性能问题或者兼容性问题，我只是从一个强迫症的角度讨论一下怎样在语法上进行一个让人满意的规定。

## 一些语言中的实现方式

#### C

```c
// write
size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );
// read
size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );
```

如你所见，C 语言里的实现其实相当简单粗暴。基本上唯一常用的方法就是二进制写入和二进制读取。当然这只是对于结构体说的，所以说你即使有一堆指针什么的他也并不会在意，统统当作二进制写进去了。所以这个方法只适用于纯数据结构体的写入，对于复杂的数据结构，基本上只能全手动管理，包括写入和读取时的对象构造，C 不提供任何形式的序列化规定或者语法糖。这个设计的确非常 C style，虽然简陋，但是完备 —— 想用你就自己实现一个呗。

#### C++

我对于 C++ 没什么研究，说起来在 C++ 环境下要做序列化一般都是用 Protobuf 或者 Boost.Serialization，但是不好意思，C++ 的大库我基本都没碰过，所以我也说不出什么。另一个原因是不同的库的实现可能差很多，所以这个大坑我也不想挖了。这里我们说点简单的，也是我比较熟悉的部分，就是重载流操作符。

```cpp
// write
friend std::ostream& operator<<(std::ostream &os, const Foo& foo) {
  // do your things
}
// write
friend std::istream& operator>>(std::istream &is, Foo &foo) {
  // do your things
}
```

这里我用的是友元的定义，当然如果你的类接口充分的话，你也完全可以不用友元，就当作普通的全局方法。说到底友元不过是一种特殊的全局方法，所以这种实现仍然是面向过程的。当读取对象的时候，你需要手动构建一个空对象，然后调用一个手动的读取函数，大概像这样：

```cpp
Foo foo1;    // original
Foo foo2;    // buffer
std::ofstream os;    // output file stream
os.open("bar");    // open file
os << foo1;    // write object
os.close();    // close stream
std::ifstream is;    // input file stream
is.open("bar");    // open file
is >> foo2;    // read object to buffer
is.close();    // close stream
std::cout << (foo1 == foo2) << std:endl;
```

这里看起来这段代码似乎很面向对象，但是注意，这里我们的读取和写入方法都是前面定义的全局函数。也就是说，我们这里用了两个全局函数假装成了流的类函数，然后悄悄的执行到了我们自定义的方法上去。值得注意的是，在 C++ 这种强调面向对象的语言里，序列化操作仍然是一个面向过程的实现。

#### Python

```python
import pickle

a = Foo()

with open('bar', 'wb') as handle:
    pickle.dump(a, handle, protocol=pickle.HIGHEST_PROTOCOL)

with open('bar', 'rb') as handle:
    b = pickle.load(handle)

print a == b
```

这段代码的内容是 Python 里用 pickle 实现的全自动序列化。值得注意的是，这里的 picker 是完全用 python 标准写的（不像 java 的黑盒子序列化）。如果你研究一下他的实现，你大概会发现在 python 里一个对象不过是一个数据块，外部也是可以任意改动的（甚至是对象的类型），我觉得这种实现完全是基于脚本语言的灵活性得以存在。

当然，你也可以手动定义序列化与反序列化行为，这时你需要手动实现一个 Pickler 和 Unpickler。这里我把自定义 Unpicker 的典型用法贴出来：

```python
class MyUnpickler(pickle.Unpickler):
    def persistent_load(self, pid):
        # do your things
```

如果没有看过文档的话，你可能不太清楚这个程序的思路，长话短说，基本就是在 `persistent_load` 这个方法里进行所有的反序列化操作，包括对象构建，还有数据读取，比较类似于一个工厂对象，他最大的弊端在于不通用，Unpicker 必然是和类绑定的。具体代码参见 [文档][1]。

#### Java

实际上我觉得 Java 的突破有相当一部分要算在反射头上，而 Java 的序列化也是和反射脱不了关系。这里我不说 Java 的全自动序列化，我觉得这一部分还是相当完整的，让我比较气愤的是 Java 对于自定义序列化的实现。你需要在类里实现这两个方法：

```java
private void writeObject(java.io.ObjectOutputStream out)
    throws IOException;
private void readObject(java.io.ObjectInputStream in)
    throws IOException,ClassNotFoundException;
```

重点在下面这个方法。当 Java 的反序列化机制运作时，虚拟机会强行构造一个相应类型的空对象，绕过所有构造函数，然后神不知鬼不觉的调用到 `readObject` 这个方法上，基于这个新构造的空对象进行序列化。这里的 `readObject` 基本上就是一个换壳的构造函数。这两个函数并不存在重载，因为你也可以查到 `Serializable` 接口里是没有这两个方法的。说起来 `writeObject` 作为函数重载还是说得过去的，而 `readObject` 这个函数完全就是类函数中的异类（他不基于一个正常的类对象），作为重载实在勉强。

## 说一点我的看法

所以问题就出在反序列化这一环上。在一个面向对象的环境里，反序列化就意味着构造对象，构造对象就意味着调用构造函数，但是构造函数不存在继承，也就意味着我永远不可能获得一个通用的办法，来适应所有类的各种构造函数。这时候你可能就要开始骂起来了：工厂类啊，他不就是为了提供一个构造函数的替代方案么！此言不虚，不过，问题又来了，我们怎么拿到工厂类的对象呢？你可能又要骂了：静态工厂类不就不需要对象么！是是是，你说的没错，但是在我见过的所有指令式面向对象的语言里，静态方法都不允许继承或者重载（大部分人给出的解释是：没必要），于是，就算我能拿到工厂类的类名，我又怎么确定它有没有我要的方法呢？这时候可选的方案就已经不多了，java 的选择就是反射，但是在 C++ 里你反射一个我看看？反射并不是面向对象必须具有的特质，但是在 java 里，如果没有反射，他就不能完整的实现反序列化。换句话说，java 为了成为一个严格的面向对象语言，把脏活全丢给了反射。

当然还有另外一个方法，如果一个对象是可以序列化的，那么他应当能用 `InputStream` 来构造。那么我们不如规定一个特殊的构造函数，接受一个 `InputStream` 来构造对象。大概像这样：

```java
public MyClass(java.io.ObjectInputStream in) {
    // do your things
}
```

但是你可能已经想到了，构造函数也是不可以继承的，也就是说，即使我们有这样一个构造函数，仍然甩不开反射，因为我们没法确定这个构造函数是存在的。

这里我们批判完 java，不如看看 C++。你可以看到相关的反序列化算法是作为全局函数注册的，毕竟 C++ 不是一个纯面向对象的语言，面向对象走不通我还可以用裸体函数那一套。再看 python 的反序列化，他其实没什么可说的，毕竟是传说中搅屎棍一般的语言，连静态函数都可以继承，接口这种东西也是不存在的，调用方法有就是有，没有就丢异常。作为一门脚本语言，他牺牲了闭包（或者说他根本不在意），反而在语法上获得了极大的灵活度。

如果有幸你读到这里，你大概会发现这其实是一篇充满着私货的吹水文，虽然我的主要范畴都在面向对象开发，但是我也越发感到面向对象范式的种种缺陷，包括 C++ 的复杂，Java (以及 JVM 上一票小弟)的繁琐。在某些大量序列化，或者是重量级的环境下（比如 Android），反射甚至成为了常态。我感觉我现在的开发思路有所转换，死抓面向对象几乎是没有出路的，更重要的反而是把程序写的统一，易读，易维护，高性能，这些都比一个范式重要得多。

共勉。

[1]: https://docs.python.org/3/library/pickle.html#pickle-persistent
