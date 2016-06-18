# optional
A Scala-like Optional class for C++11 and up.

Some interesting conversation about Scala Option types can be found here: http://danielwestheide.com/blog/2012/12/19/the-neophytes-guide-to-scala-part-5-the-option-type.html

Also, please visit the Scala Option documentation.

This class attempts to use the idioms of Scala Option but present a typical C++ collection interface so it feels natural to use.

Optional is currently immutable.  There is no plan to change this.  True to functional programming, immutable objects are a good thing and should be preferred.
