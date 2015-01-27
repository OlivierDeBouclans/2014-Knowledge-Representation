/**
 * This class describe the data structure used to
 * represent the semantic net. All the knowledge representation
 * mechanism are within these classes.
 *
 * There is no Qt related classes used here.
 * All other files are for representation and manipulation purposes.
 *
 * @author: LE BAS DE BOUCLANS Olivier
 */

#ifndef SEMANTICNET_H
#define SEMANTICNET_H

//STD include
#include <string>
#include <vector>
#include <map>

#define IN
#define OUT
#define INOUT

using namespace std;

/**
 * @class Tree
 * @brief Basic non binary tree
 */
template<class T>
struct Tree
{
  T node;
  vector<Tree<T> > child;
};

/**
 * @class Serialization
 * @brief Save/Load mechanism
 */
class Serialization
{
  public:

    Serialization() {}
    virtual ~Serialization() {}

    /**
     * @brief Convert the class into a string
     *
     * This is use for saving.
     *
     * @return A string containing the class' data
     */
    virtual string toString() const=0;

    /**
     * @brief Convert a string to a the class
     *
     * This is use for loading
     */
    virtual void setFromString(INOUT string& stringFile)=0;

    /**
     * @brief read
     * @param dest string
     * @param origine string
     *
     * Read the next input in the given string and write it on the
     * other one.
     *
     * This function is used to load an element from a string
     */
    static inline void read(OUT string& dest, IN string& origine)
    {
      int i = origine.find(";");
      dest = origine.substr(0,i);
      origine.erase(0,i+1);
    }
};


///////////////////////////////////////////////////////////////
//                   Numerical Info                          //
///////////////////////////////////////////////////////////////

#define NUM_MAX __DBL_MAX__
#define NUM_MIN __DBL_MIN__

/**
 * @class NumericalInfo
 *
 * @brief Contain all the numerical related information of an element
 *
 * Numerical information is for now only a range of possible value, with
 * a min and a max (real)
 */
class NumericalInfo: public Serialization
{
  public:
  //Constructors

    /**
     * @brief Default constructor
     *
     * The value range is set to the maxium
     */
    NumericalInfo(): dMin(NUM_MIN), dMax(NUM_MAX){}

    /**
     * @brief Constructor with a specific range
     * @param min
     * @param max
     */
    NumericalInfo(double min, double max): dMin(min), dMax(max){}

  //Serialization

    /**
     * @see Serialization::toString()
     */
    string toString() const;

    /**
     * @see Serialization::setFromString()
     */
    virtual void setFromString(INOUT string& stringFile);

  //Other methods

    /**
     * @brief operator= overload
     */
    NumericalInfo& operator=(const NumericalInfo& e);

  //Attributes
    double dMin; //Minimum of the range
    double dMax; //Maximum of the range
};




///////////////////////////////////////////////////////////////
//                    Physical Info                          //
///////////////////////////////////////////////////////////////

#define TIME_INVALID __INT64_MAX__ //Value for unknown time

/**
 * @class ConcreteInfo
 *
 * @brief Contain all the concrete related information of an element
 *
 * Concrete information is for now only a time range, with a start and
 * an end (in ms since january 1st 1970)
 */
class ConcreteInfo: public Serialization
{
  public:
  //Constructor

    /**
     * @brief Default constructor
     *
     * The time range is set to unknonw (ie invalid time)
     */
    ConcreteInfo(): iStartTime(TIME_INVALID), iEndTime(TIME_INVALID){}


    /**
     * @brief Constructor with a specific time range
     * @param start
     * @param end
     */
    ConcreteInfo(__int64 start, __int64 end): iStartTime(start), iEndTime(end){}

  //Serialization

    /**
     * @see Serialization::toString()
     */
    string toString() const;

    /**
     * @see Serialization::setFromString()
     */
    virtual void setFromString(INOUT string& stringFile);

  //Other methods

    /**
     * @brief operator= overload
     */
    ConcreteInfo& operator=(const ConcreteInfo& e);

  //Attributes
    __int64 iStartTime; //starting time in ms since 01/01/1970
    __int64 iEndTime;   //ending time in ms since 01/01/1970
};




///////////////////////////////////////////////////////////////
//                     AccessControl                         //
///////////////////////////////////////////////////////////////

class Entity;

/**
 * @class AccessControl
 *
 * @brief Describe blacklist and whitelist to the access of an element
 *
 * Only SubNode and Edge possess this AccessControl class.
 * Only Entity can be white/blacklisted
 *
 * If an entity is blacklisted, this entity will not see this specific element.
 *
 * The whitelist however can only have 1 element (the parent). Parent means that
 * this subnode/edge was created specificly for this entity (and all its descendent).
 * The subnode/edge's access is exclusive to the pParent and its not blacklisted decendent.
 * If pParent=NULL, any entity can access this element as long as it's not blacklisted.
 */
class AccessControl: public Serialization
{
  public:
  //Constructor

    /**
     * @brief Default constructor
     *
     * Blacklist and whitelist are empty at first.
     */
    AccessControl(): pParent(NULL) {}

  //Blacklist management

    /**
     * @brief Add a new entity to the blacklist
     * @param e Entity to blacklist
     *
     * The function will check if e exists and is not already in the blacklist
     */
    void blackList(Entity* e);

    /**
     * @brief Check if an entity is blacklisted;
     * @param e Entity to check
     *
     * @return True if e is blacklisted
     *         False otherwise
     */
    bool isBlackListed(Entity* e) const;

    /**
     * @brief Check if any of the entities of a vector is blacklisted;
     * @param vEntity Vector of Entity
     *
     * In pratical use, vEntity contain the hierachy of an Entity,
     * i.e. the entity and all the entities it inherits from (with a IsA relation)
     *
     * @return True if at least 1 entity of vEntity is blacklisted
     *         False otherwise
     */
    bool isBlackListed(const vector<Entity*>& vEntity) const;

  //Parent/Whitelist management

    /**
     * @brief Check if the element's parent is in a vector of Entity
     * @param vEntity vector of Entity
     *
     * In pratical use, vEntity contain the hierachy of an Entity,
     * i.e. the entity and all the entities it inherits from (with a IsA relation)
     *
     * @return True if pParent==NULL or is in vEntity
     *         False otherwise
     */
    bool isWhiteListed(const vector<Entity*>& vEntity) const;

    /**
     * @brief Search for the element's parent
     *
     * If pParent!=NULL, it will of course return pParent.
     * But if pParent==NULL, it will search for it parent,
     * i.e. the entity to which the subnode or edge is connected to.
     *
     * @return parent
     */
    virtual Entity* getParent() const;

  //Other methods

    /**
     * @brief Check if an entity can access this element
     * @param vHierachy vector of Entity
     *
     * The vector of entity contain the hierachy of a specific entity.
     *
     * @return True if the entity of hierachy vHierachy can access the element
     *         False otherwise
     */
    bool canBeAccessedBy(vector<Entity*> vHierachy) const;

  //Serialization

    /**
     * @see Serialization::toString()
     */
    string toString() const;

    /**
     * @see Serialization::setFromString()
     */
    virtual void setFromString(INOUT string& stringFile);

  //Attributes
    vector<Entity*> vBlackList; //blacklist, entities which can't access this element
    Entity* pParent;            //whitelist, entity which can access this element
};




///////////////////////////////////////////////////////////////
//                    Semantic Element                       //
///////////////////////////////////////////////////////////////

#define COOR_INVALID __INT_MAX__ //Invalid coordinate
#define TYPE_INVALID 0x00000000  //Invalid type

//Type access (see below) on SemanticElement
#define TYPE_ELEMENT(a)   ((a)&SemanticElement::TYPE)
#define TYPE_NODE(a)      ((a)&Node::TYPE)
#define TYPE_ENTITY(a)    ((a)&Entity::TYPE)
#define TYPE_SUBNODE(a)   ((a)&SubNode::TYPE)
#define TYPE_DECORATOR(a) ((a)&Decorator::TYPE)
#define TYPE_EDGE(a)      ((a)&Edge::TYPE)

class GraphElement;
class SemanticNet;

/**
 * @class SemanticElement
 *
 * @brief Base representation of all the element of the semantic net
 *
 * The SemanticElement also link the element with its GraphElement,
 * i.e. the class used to represent the element on the GUI.
 *
 * The specific type of all element is contained in 1 same 32byte word.
 * This means that we need to use a mask to access a specific subclass type.
 * Ex: to know if an SemanticElement e is a subnode or an entity (both subclasses
 * of Node), we need to use: e.type() & Node::MASK == NODE::ENTITY
 *
 * The type word is as follow:
 * 
 * (0000) (0000) (0000) (0000) (0000) (0000) (0000) (000x)
 * x=0->Invalid type (0x0000 0000)
 * x=1->Valid type   (0x0000 0001)
 *
 * (0000) (0000) (0000) (0000) (0000) (0000) (0000) (00x1) -Valid
 * x=0->Node (0x0000 0001)+(0x0000 0000)
 * x=1->Edge (0x0000 0001)+(0x0000 0002)
 *
 * (0000) (0000) (0000) (0000) (0000) (0000) (0000) (0x01) -Node
 * x=0->Entity  (0x0000 0001)+(0x0000 0000)
 * x=1->SubNode (0x0000 0001)+(0x0000 0004)
 *
 * (0000) (0000) (0000) (0000) (0000) (0000) (0000) (x101) -SubNode
 * x=0->Terminal  (0x0000 0005)+(0x0000 0000)
 * x=1->Decorator (0x0000 0005)+(0x0000 0008)
 *
 * (0000) (0000) (0000) (0000) (0000) (0000) (00xy) (1101) -Decorator
 * xy=00->Or  (0x0000 000D)+(0x0000 0000)
 * xy=01->And (0x0000 000D)+(0x0000 0010)
 * xy=10->Xor (0x0000 000D)+(0x0000 0020)
 *
 *
 * (0000) (0000) (0000) (0000) (0000) (0000) (000x) (y001) -Entity
 * xy=00->Object    (0x0000 0001)+(0x0000 0000)
 * xy=01->Attribute (0x0000 0001)+(0x0000 0008)
 * xy=10->Value     (0x0000 0001)+(0x0000 0010)
 * xy=11->Action    (0x0000 0001)+(0x0000 0018)
 *
 *
 * (0000) (0000) (0000) (0000) (0000) (0000) (0000) (0x11) -Edge
 * x=0->Has (0x0000 0003)+(0x0000 0000)
 * x=1->Is  (0x0000 0003)+(0x0000 0004)
 *
 *
 * To used polymorphisme SemanticElement need to be virtual, and the type value
 * can use some security (like not allow a Edge class to have the Entity type).
 * Therefore the setType() function is pure virtual.
 *
 * The (x,y) coordinates are stored here for conveniance. As the element can be add
 * or remove from the representation, these value are used to stored the last position
 * of the element's representation. As the GraphElement is deleted when no represented, this
 * coordinate can't not be stored by GraphElement.
 *
 */
class SemanticElement: public Serialization
{
  public:
  //Enum

    /**
     * @brief Mask to access the SemanticElement's type (with &)
     */
    static const __int32 TYPE=0x00000003;

    /**
     * @brief Element's type enum
     *
     * A semantic element is either an Edge or a Node
     */
    enum Type
    {
      NODE=0x00000001,
      EDGE=0x00000003,
    };

  //Constructor

    /**
     * @brief Default constructor
     *
     * By default, no type is specified
     *
     * @param label name of the element
     */
    SemanticElement(string label): sLabel(label), pGraphItem(NULL),  pConcrete(NULL), pNumerical(NULL), pSemanticNet(NULL), x(COOR_INVALID), y(COOR_INVALID), m_eType(TYPE_INVALID) {}

  //Destuctor

    /**
     * @brief Destructor
     *
     * Delete the addition information pointers and the GraphElement
     */
    virtual ~SemanticElement();

  //Additional information management

    /**
     * @brief Add or Remove the additional concrete information
     * @param bConcrete
     *
     * This function create or delete a ConcreteInfo instance
     */
    void setConcrete(bool bConcrete);

    /**
     * @brief Set the time range for the concrete data
     * @param start start time
     * @param end end time
     *
     * If the element doesn't have ConcreteInfo (i.e. pConcrete==NULL),
     * the function create it.
     */
    void setConcrete(__int64 start, __int64 end);


    /**
     * @brief Add or Remove the additional numerical information
     * @param bNumerical
     *
     * This function create or delete a NumericalInfo instance
     */
    void setNumerical(bool bNumerical);

    /**
     * @brief Set the value range for the numerical data
     * @param min
     * @param max
     *
     * If the element doesn't have ConcreteInfo (i.e. pConcrete==NULL),
     * the function create it.
     */
    void setNumerical(double min, double max);

  //Type management

    /**
     * @brief Get the Element's type
     *
     * This value is set in the correspoiding type class
     *
     * @return The element type
     */
    inline const __int32& type() const {return m_eType;}

    /**
     * @brief Set the Element's type
     * @param type
     *
     * This function check is the given type is valid with the current class.
     * As this check depends on the class, this function is pure virtual
     */
    virtual void setType(__int32 type)=0;

    /**
     * @brief Get the name of the type
     * @return A string containing the type name
     */
    string typeToString() const;

  //Attributes
    string sLabel; //Name of the element

    GraphElement* pGraphItem;  //Graphical representation of the element
    ConcreteInfo* pConcrete;   //Concrete  data (if needed)
    NumericalInfo* pNumerical; //Numerical data (if needed)

    SemanticNet* pSemanticNet; //SemanticNet in wich the element belong

    int x,y; //Last known position of the graphical representation

  protected:
    __int32 m_eType; //Element's type
};



///////////////////////////////////////////////////////////////
//                           Node                            //
///////////////////////////////////////////////////////////////

#define ID_INVALID 0 //unspecified ID

class Edge;
class SubNode;

/**
 * @class Node
 *
 * @brief Nodes are the element which can be connected to each other
 * by Edges
 *
 * A node has an ID. ID is a 64byte word specific to the node, used to
 * identify it. The ID is given by the semantic net, therefore not part
 * of the constructor.
 *
 * The purpose of ID is for saving, and more specificaly for edge's saving
 * and loading. Edges need a mean to identify its extremities, and as the label
 * isn't unique, a numerical ID is used.
 *
 * A node contain a list of all its subnode and edges.
 * Edges' list is split in 2: incoming and outcoming edges.
 * This is done to facilitate some processing like hierarchy or composition
 * where only outcoming edges are relevant (incoming edges usaly aren't usefull
 * outside graphical representation)
 */
class Node: public SemanticElement
{
  public:
  //Enum

    /**
     * @brief Mask to access the Node's type
     */
    static const __int32 TYPE=0x00000007;

    /**
     * @brief Element's type enum
     *
     * A Node is either an Edge or a Node
     */
    enum Type
    {
      ENTITY=NODE+0x00000000,
      SUBNODE=NODE+0x00000004
    };

  //Constructor

    /**
     * @brief Default constructor
     * @param label
     *
     * By default, no ID is given
     */
    Node(string label): SemanticElement(label), ID(ID_INVALID) {}

  //Destructor

    /**
     * @brief Destructor
     *
     * Destroy all the edges and subnodes.
     * An Edge's other extremity is first disconnected before deletion.
     *
     * Ex: (Node A)--(Edge e)->(Node B)
     * When deleting A, e is removed from the edge's list of B (i.e.
     * disconnected), then e is destroy, then A.
     */
    virtual ~Node();

  //Edges management

    /**
     * @brief Get the incoming edge
     *
     * Incoming edges: (Node A)<--(Edge e)--...
     *
     * @return vector of edge
     */
    const vector<Edge*>& getInEdge() const {return m_vInEdge;}

    /**
     * @brief Get the outcoming edge
     *
     * Outcoming edges: (Node A)--(Edge e)-->...
     *
     * @return vector of edge
     */
    const vector<Edge*>& getOutEdge() const {return m_vOutEdge;}

    /**
     * @brief Add an edge to the list
     * @param e Edge to add
     *
     * The edge's extremities are already set.
     * The function check if the edge do connect this node with another,
     * and then add it to the relevant list (outcoming or incoming edge)
     */
    void add(Edge* e);

    /**
     * @brief Disconnect an edge from the node
     * @param e Edge to diconnect
     *
     * This function DO NOT delete the edge. It just remove it from
     * its list and set e's coresponding extremities to NULL.
     *
     * This function is used by the other side of the edge when the
     * node is being destroyed.
     */
    void disconnect(Edge* e);

  //SubNode management

    /**
     * @brief Get the subnodes
     * @return vector of subnode
     */
    const vector<SubNode*>& getSubNode() const {return m_vSubNode;}

    /**
     * @brief Add a subnode to the list
     * @param e Edge to add
     *
     * Set the subnodes owner to this node.
     */
    void add(SubNode* subnode);

    /**
     * @brief Disconnect a subnode from the node
     * @param subnode SubNode to diconnect
     *
     * This function DO NOT delete the subnode. It just remove it from
     * its list and set subnode's owner to NULL.
     *
     * This function is used when deleting a subnode
     */
    void disconnect(SubNode* subnode);

    /**
     * @brief Remove a subnode
     * @param subnode
     *
     * This function DO delete the subnode
     */
    void remove(SubNode* subnode);

  //Type management

    /**
     * @brief Get the name of the type
     * @return A string containing the type name
     */
    string typeToString() const;

  //Other functions

    /**
     * @brief Get the node's composition knowing an entity hierarchy
     * @param vHierarchy
     *
     * The hierachy is used to now if the entity can access the current node
     * edges and subnodes.
     *
     * @return Tree of nodes
     */
    Tree<Node*> getComposition(vector<Entity*> vHierarchy);

  //Attribute
    unsigned __int64 ID; //Node's unique ID

  protected:
    vector<Edge*> m_vInEdge;     //Incoming Edges
    vector<Edge*> m_vOutEdge;    //Outcoming Edges
    vector<SubNode*> m_vSubNode; //Subnodes

};



///////////////////////////////////////////////////////////////
//                        Sub Node                           //
///////////////////////////////////////////////////////////////

class GraphSubNode;
class Entity;

/**
 * @class SubNode
 *
 * @brief A SubNode is a node belonging to another Node
 *
 * It correspond either to Decorator or Terminal.
 * A subnode is therefore just a node with a pointer
 * to its owner, and inheriting the AccessControl class.
 *
 */
class SubNode: public Node, public AccessControl
{
  public:
  //Enum

    /**
     * @brief Mask to access the SubNode's type
     */
    static const __int32 TYPE=0x0000000F;

    /**
     * @brief Element's type enum
     *
     * A SubNode is either an Edge or a Node
     */
    enum Type
    {
      TERMINAL=SUBNODE+0x00000000,
      DECORATOR=SUBNODE+0x00000008
    };

  //Constructor

    /**
     * @brief Default constructor
     * @param label
     * @param owner
     *
     * An ower must be provided to create a subnode
     */
    SubNode(string label, Node* owner): Node(label), pOwner(owner) {}

  //Destructor

    /**
     * @brief Destructor
     *
     * The node's destructor is enough. Just need to remove the
     * subnode from the owner list.
     */
    virtual ~SubNode();

  //Type management

    /**
     * @brief Get the name of the type
     * @return A string containing the type name
     */
    string typeToString() const;

  //Other functions

    /**
     * @brief get the Subnode's graphical representation
     * @return pGraphItem cast in the adequate type
     */
    inline GraphSubNode *getGraph() const {return (GraphSubNode*)pGraphItem;}

    /**
     * @brief Return the parent
     * @return Parent
     * @see AccessControl::getParent()
     */
    Entity* getParent() const;

    /**
     * @see SemanticElement::toString()
     *
     * A subnode still need to implement the SemanticElement
     * toString() function. This is here to avoid ambiguous use
     * with the AccessControl::toString()
     */
    virtual string toString() const=0;

    /**
     * @brief Constructor from string
     * @param stringFile :string to read
     *
     * Construct a subnode by reading a string.
     */
     static SubNode* fromString(string& stringFile);

  //Attribute
    Node* pOwner; //owner of the subnode (can be a subnode)
};



///////////////////////////////////////////////////////////////
//                       Decorator                           //
///////////////////////////////////////////////////////////////

/**
 * @class Decorator
 * @brief A decorator is a subnode with additional information
 *
 * The purpose of decorators is to add a specific relation between
 * edges in a group of edge. For example, a XOR relation between each
 * edge or the fact that all the edge of the group (subnode) represent
 * a partition.
 *
 * For now there are 3 types of decorator: OR, AND and XOR.
 *
 */
class Decorator: public SubNode
{
  public:
  //Enum

    /**
     * @brief Mask to access the decorator's type
     */
    static const __int32 TYPE=0x0000003F;

    /**
     * @brief Decorator's type enum
     *
     * A Decorator is either Or, And or Xor
     */
    enum Type
    {
      OR=DECORATOR+0x00000000,
      AND=DECORATOR+0x00000010,
      XOR=DECORATOR+0x00000020
    };

  //Constructor

    /**
     * @brief Defautl constructor
     * @param owner
     * @param type :OR, AND or XOR
     * @param bPartition :if the decorator describe a partition
     *
     * The type is set by setType() to ensure a valid type
     * By default, bPartition is false
     */
    Decorator(Node* owner, Type type, bool bPartition = false);

  //Type Management

    /**
     * @brief Implementation of setType()
     * @see SemanticElement::setType()
     */
    void setType(__int32 type);

    /**
     * @brief Implementation of typeToString()
     * @see SemanticElement::typeToString()
     */
    string typeToString() const;

  //Serialization

    /**
     * @see Serialization::toString()
     */
    string toString() const;

    /**
     * @see Serialization::setFromString()
     */
    virtual void setFromString(INOUT string& stringFile);

  //Other functions

    /**
     * @brief operator= overload
     */
    Decorator& operator=(const Decorator& e);

  //Attribute
    bool bPartition; //does the decorator describe a partition
};




///////////////////////////////////////////////////////////////
//                        Terminal                           //
///////////////////////////////////////////////////////////////

/**
 * @class Terminal
 *
 * @brief A decorator is a special subnode of an action
 *
 * The purpose of terminals is to be parameters of action.
 * Ex: actor, subject, tool, location, etc.
 *
 * Different types of terminals could bring additional information.
 * Ex: actor of an action means that the actor entity has the same
 * location as the action.
 *
 * For now there is no special meanings, terminals are juste subnodes
 * with a label.
 */
class Terminal: public SubNode
{
  public:
  //Constructor

    /**
     * @brief Default constructor
     * @param label
     * @param owner
     */
    Terminal(string label, Node* owner): SubNode(label,owner) {setType(TERMINAL);}

  //Type Management

    /**
     * @brief Implementation of setType()
     * @see SemanticElement::setType()
     */
    void setType(__int32 type);

    /**
     * @brief Implementation of typeToString()
     * @see SemanticElement::typeToString()
     */
    string typeToString() const;

  //Serialization

    /**
     * @see Serialization::toString()
     */
    string toString() const;

    /**
     * @see Serialization::setFromString()
     */
    virtual void setFromString(INOUT string& stringFile);
};




///////////////////////////////////////////////////////////////
//                         Entity                            //
///////////////////////////////////////////////////////////////

/**
 * @class Entity
 *
 * @brief Entities are the main node of the semantic net
 *
 * Basicaly all main elements of the semantic net are entities.
 * They represent the concepts: object, attribute, value or action.
 *
 * Entity has some specific functions like hierarchy or composition
 * (non aplicable for subnodes)
 */
class Entity: public Node
{
  public:
  //Enum

    /**
     * @brief Mask to access the Entity's type
     */
    static const __int32 TYPE=0x0000001F;

    /**
     * @brief Entity's type enum
     *
     * A Entity is either an Object, Attribute, Value or Action
     */
    enum Type
    {
      OBJECT=NODE+0x00000000,
      ATTRIBUTE=NODE+0x00000008,
      VALUE=NODE+0x00000010,
      ACTION=NODE+0x00000018
    };

  //Constructor

    /**
     * @brief Default constructor
     * @param label
     * @param type
     *
     * The type is checked to ensure it's a right one.
     */
    Entity(string label, Type type) :  Node(label) {setType(type);}

  //Destructor

    /**
     * @brief Destructor
     *
     * As Entities are stored in the semantic Net, it needs to be
     * removed from it at deletion
     */
    ~Entity();

  //Type Management

    /**
     * @brief Implementation of setType()
     * @see SemanticElement::setType()
     */
    void setType(__int32 type);

    /**
     * @brief Implementation of typeToString()
     * @see SemanticElement::typeToString()
     */
    string typeToString() const;

  //Data mining

    /**
     * @brief Get the hierachy of an entity
     *
     * The hierarchy is all the entities this entity is linked to
     * by a IsA relation. It's used to describe the entity construction.
     *
     * @return Tree of entity
     */
    Tree<Entity*> getHierarchy();

    /**
     * @brief Get the hierarchy in vector form
     * @return vector of entities
     *
     * This function is use to enable an iterative computation of AccessControl
     * with a linear hierarchy.
     */
    vector<Entity*> getVectorHierachy();

    /**
     * @brief Get the composition of the Entity
     *
     * The composition is all the entities this entity have, i.e all
     * the entities this entity is linked to by a Has relation.
     * It is used to decribe the parts and property this entity has.
     *
     * As the decorator give additional information on parts or attributes,
     * it's included in the tree, therefore becoming a tree of node.
     *
     * @return Tree of Node
     */
    Tree<Node*> getComposition();

  //Serialization

    /**
     * @see Serialization::toString()
     */
    string toString() const;

    /**
     * @see Serialization::setFromString()
     */
    virtual void setFromString(INOUT string& stringFile);

  //Other functions

    /**
     * @brief operator= overload
     */
    Entity& operator=(const Entity& e);

  protected:

    /**
     * @brief Convert a tree to a Vector
     * @param tree IN
     * @param vector OUT
     *
     * This function convet a Tree of Node (stucture with depth) into
     * a vector of Entity (structure in line).
     *
     */
    void treeToVector(IN Tree<Entity*>& tree, OUT vector<Entity*>& vector) const;
};



///////////////////////////////////////////////////////////////
//                           Edge                            //
///////////////////////////////////////////////////////////////

#define EDGE_INVALID "INVALID"

class GraphEdge;

/**
 * @class Edge
 *
 * @brief Edge represents the links between 2 nodes
 *
 * For now only 2 types of edge are implemented: IS or HAS
 */
class Edge: public SemanticElement, public AccessControl
{
  public:
  //Enum

    /**
     * @brief Mask to access the edge's type
     */
    static const __int32 TYPE=0x00000007;

    /**
     * @brief Edge's type enum
     *
     * An edge is either Has or Is for now
     */
    enum Type
    {
      HAS=EDGE+0x00000000,
      IS=EDGE+0x00000004
    };

  //Constructor

    /**
     * @brief Default constructor
     * @param from
     * @param type Is or Has
     * @param to
     *
     * The type is checked to see if it's adequate.
     * By default card is 1.
     */
    Edge(Node* From, Type type, Node* To, int card=1);

    /**
     * @brief Destructor
     *
     * Disconnect the edge from the nodes before destroying it
     * Also disconnect it from the semanticNet
     */
    virtual ~Edge();

    /**
     * @brief Create an edge of the according type
     * @param from
     * @param type
     * @param to
     *
     * This is an edge factory. This static function create the
     * subclass corresponding to the inputed type (EdgeIS, or EdgeHas)
     *
     * @return A new edge of corresponding type
     */
    static Edge* newEdge(Node *From, Type type, Node *To);

  //Access Control

    /**
     * @brief Return the parent
     * @return Parent
     * @see AccessControl::getParent()
     */
    Entity* getParent() const;

    /**
     * @brief check if the edge is valid
     *
     * The edge is valid as long as its label!=EDGE_INVALID.
     * The label of an edge is created according to some conditions
     * Ex: a Has edge will be "Has Attribute" if it links an attribute
     * and "Has" if it links an object.
     *
     * This creation of label check if this edge is plausible. Therefore
     * if the label remain EDGE_INVALID, the edge is invalid.
     *
     * @return True if the edge is valid
     *         False otherwise
     */
    inline bool isValid() const {return sLabel!=EDGE_INVALID;}

  //Type Management

    /**
     * @brief Implementation of setType()
     * @see SemanticElement::setType()
     */
    void setType(__int32 type);

    /**
     * @brief Implementation of typeToString()
     * @see SemanticElement::typeToString()
     */
    string typeToString() const;

  //Serialization

    /**
     * @see Serialization::toString()
     */
    string toString() const;

    /**
     * @see Serialization::setFromString()
     */
    virtual void setFromString(INOUT string& stringFile);

  //Other functions

    /**
     * @brief Init the edge label
     *
     * This function check the from and to of the edge to determine
     * wich label is suitable for the edge, and therefore if this edge
     * is plausible.
     *
     * As condition depend on the edge's type, this is pure virtual
     *
     * @see Edge::isValid()
     */
    virtual string createLabel() const=0;

    /**
     * @brief get the edge's graphical representation
     * @return pGraphItem cast in the adequate type
     */
    inline GraphEdge *getGraph() const {return (GraphEdge*)pGraphItem;}

    /**
     * @brief operator= overload
     */
    Edge& operator=(const Edge& e);

    /**
     * @brief Constructor from string
     * @param stringFile :string to read
     *
     * Construct a edge by reading a string.
     */
     static Edge* fromString(string& stringFile);

  //Attributes
    Node *from; //start of the edge
    Node *to;   //end of the edge
    int  iCard; //cardinal
};



///////////////////////////////////////////////////////////////
//                        EdgeIs                             //
///////////////////////////////////////////////////////////////

/**
 * @class EdgeIs
 * @brief Describe a hierachy relation
 *
 * EdgeIs basicaly describe a typeOf relation
 * Ex: Human--IsA->Animal
 *     Paul--InstanceOf->Human
 */
class EdgeIs : public Edge
{
  public:

    /**
     * @brief Default construcor
     * @param from
     * @param to
     */
    EdgeIs(Node* From, Node *To): Edge(From,IS,To) {sLabel=createLabel();}

    /**
     * @see Edge::makeLabel()
     */
    string createLabel() const;
};




///////////////////////////////////////////////////////////////
//                        EdgeHas                            //
///////////////////////////////////////////////////////////////

/**
 * @class EdgeHas
 * @brief Describe a composition relation
 *
 * EdgeHas basicaly describe a possesion relation
 * Ex: Human--Has 2->Leg
 *     Animal--Has Attribute->Sex
 *     Sex--Has Attribute->Male
 */
class EdgeHas : public Edge
{
  public:

    /**
     * @brief Default construcor
     * @param from
     * @param to
     */
    EdgeHas(Node* From, Node *To): Edge(From,HAS,To) {sLabel=createLabel();}

    /**
     * @see Edge::makeLabel()
     */
    string createLabel() const;
};




///////////////////////////////////////////////////////////////
//                      Semantic net                         //
///////////////////////////////////////////////////////////////

/**
 * @class SemanticNet
 * @brief This class regroup SemanticElements to from a SemanticNet
 *
 * Semanticnet is used to know all the elements without having to do
 * a depth search.
 * It also keep track of element's ID in order to perfore saving/loading.
 */
class SemanticNet
{
  public:

  //Constructor

    /**
     * @brief Default constructor
     *
     * An empty semantic net with a starting ID invalid.
     */
    SemanticNet() : m_iLastID(ID_INVALID) {}

  //Destructor

    /**
     * @brief Destructor
     *
     * Remove all elements of the semantic net
     */
    ~SemanticNet();

  //Accessor

    /**
     * @brief Get the semantic net's entities
     * @return vector of entity
     */
    const vector<Entity*>& getEntity() const {return m_vEntity;}

    /**
     * @brief Get the semantic net's edges
     * @return vector of entity
     */
    const vector<Edge*>& getEdge() const {return m_vEdge;}

  //Editing functions

    /**
     * @brief Remove an edge
     * @param edge
     *
     * This function DO NOT delete the edge. It only disconnect it.
     */
    void remove(Edge* edge);


    /**
     * @brief Remove an entity
     * @param edge
     *
     * This function DO NOT delete the entity. It only disconnect it.
     */
    void remove(Entity* entity);

    /**
     * @brief Delete all elements of the semantic net
     *
     * Actualy DO delete all the elements.
     */
    void deleteAll();

    /**
     * @brief Add Node
     * @param Node
     * @param ID
     *
     * Add a node to the semantic net.
     * If the node is a Entity, it will added to the entities's vector
     * If no ID is specified, it will chose an id for the entity.
     */
    void add(Node* e, unsigned __int64 id=ID_INVALID);

    /**
     * @brief Add edge
     * @param edge
     *
     * Add an edge to the semantic net.
     * If no ID is specified, it will chose an id for the entity.
     */
    void add(Edge* s);


   //Saving/Loading functions

    /**
     * @brief Save the current semantic net
     * @param path
     *
     * This function serialize all the elements of the semantic net
     * in a text file.
     */
    void saveNet(string path);

    /**
     * @brief Open a semantic net
     * @param path
     */
    void openNet(string path);


    void IDtoEntity(SubNode*,std::map<unsigned __int64, Node*>&);
    void IDtoEntity(Edge*,std::map<unsigned __int64, Node*>&);




   //Other function

    /**
     * @brief Change the owner of a subnode
     * @param subnode
     * @param newOwner
     */
    void changeOwner(SubNode* subnode,Node* newOwner);

  protected:
  //Attributes
    vector<Entity*> m_vEntity; //list of entities
    vector<Edge*>   m_vEdge;   //list of edges

    unsigned __int64 m_iLastID;//highest ID assign in the net
};

#endif // SEMANTICNET_H
































