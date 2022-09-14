# The Visitor Pattern
Visitor is a little clumsy and not very popular, but it has some power in select situations.

It's not a pattern that we really need in a language with proper reflection, like Java. But in C++, we don't have reflection, so there's a need.

# Usage
Imagine we have a deep tree of "POJOs" -- Plain Old Java Objects -- but in whatever language (such as C++). And now imagine that you want to be able to dump your tree as JSON.

You could do what we've done -- add a toJSON() method to each of the objects and call it a day. And that's fine. But it begins to grow unwieldy if you want to add several more methods such as toYaml() and toXML(). Now the POJOs are starting to know a bit too much about how they might be stored.

One way to move the methods out of the POJO classes is with the visitor pattern.

     class Visitor {
     public:
      virtual void visitMyPojo(MyPojo &) = 0;
     };

     class MyPojo {
     public:
         void accept(Visitor &v) { v.visitMyPojo(*this) }
     };

In this example, Visitor should actually be an abstract base class, and you would implement Visitor_ToJSON, Visitor_ToXML, and the like as concrete subclasses.

# A Little More Complexity

Now, in this example, we don't need to visitor class. We can iterate over a vector of MyPojo objects, and we don't need this abstractiong to build our JSON. But imagine a slightly more complicated situation. For instance, the one time I've actually used the Visitor pattern was when using ANTLR (which is a modern version of Lex and Yacc). With ANTLR, you get a vector of tokens. The tokens all inherit from a common subclass, but you might have hundreds of types of tokens.

And because we're not putting the logic inside Token, but inside Visitor, you can't just use polymorphism. You can't do:

    class Token {
      ...
      virtual void generateAssembly();
    }

You might not always be generating assembly. You might want to simply dump the objects. Or you might generate for different architectures. Or whatever. So the logic for what to do is in the Visitor.

But you can't do this, either:

    class Visitor {
    ...
       void generateAssembly(Token &);
    }

You can't do that because you won't know what token you have. So you'd end up doing something like:

    switch(token.type()) {
      case Foo: handleFoo(token); break;
      case Bar: handleBar(token); break;
    }

And that's if you actually have a type to compare.

So instead you use the visitor pattern, and you define visitFoo, visitBar, et cetera. visitFoo needs to know what to do -- he knows he has a Foo.

# Usability
I think doing this for JSON conversion is more code, but it DOES isolate the JSON generation from the POJO. I'm not sure that's an improvement if you're only generating JSON. But if we were generating multiple output formats, moving the code out of the POJOs has value.

Ultimately, I think adding reflection to C++ -- somehow -- would help for a lot of this. Reflection makes output conversion a lot easier. So a different pattern (not one of the standard ones) where each class has a "whatFieldsDoYouOwn" call to implement a sort of reflection would enable the POJOs to be converted.
