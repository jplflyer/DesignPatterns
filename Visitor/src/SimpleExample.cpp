#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using std::cout;
using std::endl;
using std::string;

using JSON = nlohmann::json;
using JSON_Pointer = std::shared_ptr<JSON>;
using JSON_Vector = std::vector<JSON_Pointer>;

class Person;
class Pet;

/**
 * This defines the visitor INTERFACE. Yeah, yeah, it's C++ and they have classes,
 * but treat it like an interface.
 */
class Visitor {
public:
    virtual void visitPerson(const Person &) = 0;
    virtual void visitPet(const Pet &) = 0;
};

/**
 * Any object that can be visited should implement this interface.
 */
class CanVisit {
public:
    virtual void accept(Visitor &) = 0;
};


//======================================================================
// And here we create some classes that can be visited.
//======================================================================
class Person: public CanVisit {
public:
    typedef std::shared_ptr<Person> Pointer;
    typedef std::vector<Pointer> Vector;

    Person() = default;
    Person(const std::string &n, int a)
        : name(n), age(a)
    {
    }

    void accept(Visitor &v) override {
        v.visitPerson(*this);
    }

    std::string name;
    int age = 21;
    std::vector<std::shared_ptr<Pet>> pets;
};

class Pet: public CanVisit {
public:
    typedef std::shared_ptr<Pet> Pointer;
    typedef std::vector<Pointer> Vector;

    Pet();
    Pet(const std::string &n, const std::string &b)
        : name(n), breed(b)
    {
    }

    void accept(Visitor &v) override {
        v.visitPet(*this);
    }

    std::string name;
    std::string breed;
};

/**
 * This is the concrete implementation -- someone who applies the Visitor
 * to solve a specific problem. You could readily have other visitors that
 * rely on the same pattern to do something different.
 */
class JSONVisitor: public Visitor {
public:
    void visitPerson(const Person & person) override {
        if (currentJSON == nullptr) {
            currentJSON = std::make_shared<JSON>();
            json = currentJSON;
        }

        *currentJSON = JSON::object();

        (*currentJSON)["name"] = person.name;
        (*currentJSON)["age"] = person.age;

        if (person.pets.size() > 0) {
            JSON_Pointer me = currentJSON;
            currentJSON = std::make_shared<JSON>();

            visitPetVector(person.pets);

            (*me)["pets"] = *currentJSON;
            currentJSON = me;
        }
    }

    void visitPet(const Pet & pet) override {
        if (currentJSON == nullptr) {
            currentJSON = std::make_shared<JSON>();
            json = currentJSON;
        }

        *currentJSON = JSON::object();

        (*currentJSON)["name"] = pet.name;
        (*currentJSON)["breed"] = pet.breed;
    }

    void visitPersonVector(const Person::Vector &vec) {
        if (currentJSON == nullptr) {
            currentJSON = std::make_shared<JSON>();
            json = currentJSON;
        }

        *currentJSON = JSON::array();
        JSON_Pointer myVec = currentJSON;

        for (const Person::Pointer & person: vec) {
            currentJSON = std::make_shared<JSON>();
            *currentJSON = JSON::object();
            person->accept(*this);
            myVec->push_back(*currentJSON);
        }

        currentJSON = myVec;
    }

    void visitPetVector(const Pet::Vector &vec) {
        if (currentJSON == nullptr) {
            currentJSON = std::make_shared<JSON>();
            json = currentJSON;
        }

        *currentJSON = JSON::array();
        JSON_Pointer myVec = currentJSON;

        for (const Pet::Pointer & pet: vec) {
            currentJSON = std::make_shared<JSON>();
            *currentJSON = JSON::object();
            pet->accept(*this);
            myVec->push_back(*currentJSON);
        }

        currentJSON = myVec;
    }

    JSON_Pointer currentJSON = nullptr;
    JSON_Pointer json = nullptr;
};

//======================================================================
// Let's do it.
//======================================================================

int main()
{
    Person::Vector people;
    people.push_back(std::make_shared<Person>("Joe", 60));
    people.push_back(std::make_shared<Person>("Fred", 29));
    people.push_back(std::make_shared<Person>("Sally", 37));

    people[0]->pets.push_back( std::make_shared<Pet>("Newton", "Yellow Lab") );
    people[0]->pets.push_back( std::make_shared<Pet>("Figlet", "Dachshund mix") );

    JSONVisitor visitor;

    visitor.visitPersonVector(people);

    cout << visitor.json->dump(2) << "\n";

    return 0;
}
