# Dependency Injection Exercise

This repository does not contain any base code. Youd need to work from scratch. 

So what are you asked to do?
* Create an interface named **CompactDisc** with a void method called **play()**
* Create a class that implements the former interface that represents a concrete CD with atrributes *name* (ex: The dark side  of the moon) and *artist* (Pink Floyd)
* Implement the *play* method so that it prints the disc name and artist to the console (*System.out.println*)
* Create a class **CDPlayer** that has as an attribute a *CompactDisc* and a setter for the attribute. Also a method **play()** that calls the same method form the *CompactDisc*.
* Create a configuration class so that beans for both classes are created and the dependencies correctly injected
* Modify the application class with the *main()* method to see the whole thing working
