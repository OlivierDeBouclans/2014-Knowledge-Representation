#include "SemanticNet.h"
#include "GraphElements.h"

#include <fstream>
#include <assert.h>
#include <sstream>

///////////////////////////////////////////////////////////////
//                   Numerical Info                          //
///////////////////////////////////////////////////////////////

string NumericalInfo::toString() const
{
  ostringstream s;
  s.precision(21); //preserve all the double's precision

  s << dMin << ";";
  s << dMax << ";";

  return s.str();
}

//------------------------------------------------------------

void NumericalInfo::setFromString(string& stringFile)
{
  string sTemp;
  read(sTemp,stringFile);
  dMin=atof(sTemp.c_str());

  read(sTemp,stringFile);
  dMax=atof(sTemp.c_str());
}

//------------------------------------------------------------

NumericalInfo& NumericalInfo::operator=(const NumericalInfo& e)
{
  dMin=e.dMin;
  dMax=e.dMax;

  return *this;
}



///////////////////////////////////////////////////////////////
//                    Physical Info                          //
///////////////////////////////////////////////////////////////

string ConcreteInfo::toString() const
{
  ostringstream s;

  s << iStartTime << ";";
  s << iEndTime << ";";

  return s.str();
}

//------------------------------------------------------------

void ConcreteInfo::setFromString(string& stringFile)
{
  string sTemp;
  read(sTemp,stringFile);
  iStartTime=atoll(sTemp.c_str());

  read(sTemp,stringFile);
  iEndTime=atoll(sTemp.c_str());
}

//------------------------------------------------------------

ConcreteInfo& ConcreteInfo::operator=(const ConcreteInfo& e)
{
  iStartTime=e.iStartTime;
  iEndTime=e.iEndTime;

  return *this;
}



///////////////////////////////////////////////////////////////
//                    AccessControl                          //
///////////////////////////////////////////////////////////////

void AccessControl::blackList(Entity* e)
{
  if(e && !isBlackListed(e))
    vBlackList.push_back(e);
}

//------------------------------------------------------------

bool AccessControl::isBlackListed(Entity* e) const
{
  if(!e)
    return false;

  for(unsigned int i=0; i<vBlackList.size(); ++i)
    if(vBlackList[i]==e)
      return true;

  return false;
}

//------------------------------------------------------------

bool AccessControl::isBlackListed(const vector<Entity*>& vEntity) const
{
  if(vEntity.empty())
    return false;

  unsigned int i=0;

  //go trough vEntity until it reach the end or it found a blacklisted entity
  while( i<vEntity.size() && !isBlackListed(vEntity[i]))
    i++;

  //if it stoped before the vector's end, a blacklisted entity has been found
  return i<vEntity.size();
}

//------------------------------------------------------------

bool AccessControl::isWhiteListed(const vector<Entity*>& vEntity) const
{
  if(pParent==NULL)
    return true;

  for(unsigned int i=0; i<vEntity.size(); ++i)
  if(vEntity[i]==pParent)
    return true;

  return false;
}

//------------------------------------------------------------

Entity*AccessControl::getParent() const
{
  return pParent;
}

//------------------------------------------------------------

bool AccessControl::canBeAccessedBy(vector<Entity*> vHierachy) const
{
  return !isBlackListed(vHierachy) && isWhiteListed(vHierachy);
}

//------------------------------------------------------------

string AccessControl::toString() const
{
  ostringstream s;

  s << (pParent?pParent->ID:0) << ";";

  s << vBlackList.size() << ";";

  for(unsigned int i=0; i<vBlackList.size(); ++i)
    s << vBlackList[i]->ID << ";";

  return s.str();
}

//------------------------------------------------------------

void AccessControl::setFromString(string& stringFile)
{
  string sTemp;
  read(sTemp,stringFile);

  int parentID= atoi(sTemp.c_str());
  pParent=(Entity*)parentID;

  read(sTemp,stringFile);
  int blackList = atoi(sTemp.c_str());

  for(int i=0; i<blackList; i++)
    {
      read(sTemp,stringFile);
      int blackListID = atoi(sTemp.c_str());
      vBlackList.push_back((Entity*)blackListID);
    }
}




///////////////////////////////////////////////////////////////
//                    Semantic Element                       //
///////////////////////////////////////////////////////////////

SemanticElement::~SemanticElement()
{
  delete pGraphItem;
  delete pConcrete;
  delete pNumerical;
}

//------------------------------------------------------------

void SemanticElement::setConcrete(bool bConcrete)
{
  if(bConcrete && !pConcrete)
      pConcrete = new ConcreteInfo();
  else if(!bConcrete && pConcrete)
    {
      delete pConcrete;
      pConcrete = NULL;
    }
}

//------------------------------------------------------------

void SemanticElement::setConcrete(__int64 start, __int64 end)
{
  if(!pConcrete)
    pConcrete = new ConcreteInfo(start,end);
  else
    {
      pConcrete->iStartTime = start;
      pConcrete->iEndTime = end;
    }
}

//------------------------------------------------------------

void SemanticElement::setNumerical(bool bNumerical)
{
  if(bNumerical && !pNumerical)
      pNumerical = new NumericalInfo();
  else if(!bNumerical && pNumerical)
    {
      delete pNumerical;
      pNumerical = NULL;
    }
}

//------------------------------------------------------------

void SemanticElement::setNumerical(double min, double max)
{
  if(!pNumerical)
    pNumerical = new NumericalInfo(min,max);
  else
    {
      pNumerical->dMin = min;
      pNumerical->dMax = max;
    }
}

//------------------------------------------------------------

string SemanticElement::typeToString() const
{
  switch(TYPE_ELEMENT(m_eType))
    {
      case NODE:
        return "Node";
      break;

      case EDGE:
        return "Edge";
      break;

      default:
        return "";
      break;
    }
}



///////////////////////////////////////////////////////////////
//                            Node                           //
///////////////////////////////////////////////////////////////

Node::~Node()
{
  //e is A's incoming edge means e.to==A
  for(unsigned int i=0; i<m_vInEdge.size(); ++i)
    {
      m_vInEdge[i]->from->disconnect(m_vInEdge[i]); //disconnect the other side
      m_vInEdge[i]->to=NULL; //prevent the edge from tying to disconnect itself
      delete m_vInEdge[i];
    }

  //e is A's outcoming edge means e.from==A
  for(unsigned int i=0; i<m_vOutEdge.size(); ++i)
    {
      m_vOutEdge[i]->to->disconnect(m_vOutEdge[i]); //disconnect the other side
      m_vOutEdge[i]->from=NULL;//prevent the edge from tying to disconnect itself
      delete m_vOutEdge[i];
    }

  for(unsigned int i=0; i<m_vSubNode.size(); ++i)
    {
      m_vSubNode[i]->pOwner=NULL; //prevent the subnode to disconnect itself
      delete m_vSubNode[i];
    }
}

//------------------------------------------------------------

void Node::add(Edge *edge)
{
  //Ensure edge is valid
  if(!edge||!edge->from||!edge->to)
    return;

  //And not a self relation
  if(edge->from==edge->to)
    return;

  if(edge->from == this)
    {
      //And not already present
      for(unsigned int i=0; i<m_vOutEdge.size(); ++i)
        if(m_vOutEdge[i]==edge)
            return;

      m_vOutEdge.push_back(edge);
    }
  else if(edge->to == this)
    {
      //And not already present
      for(unsigned int i=0; i<m_vInEdge.size(); ++i)
        if(m_vInEdge[i]==edge)
            return;

      m_vInEdge.push_back(edge);
    }
}

//------------------------------------------------------------

void Node::disconnect(Edge *edge)
{
  if(!edge)
    return;

  //edge.from==this means edge is an outcoming edge
  if(edge->from==this)
    {
      for(unsigned int i=0; i<m_vOutEdge.size(); ++i)
        if(m_vOutEdge[i] == edge)
          {
            m_vOutEdge.erase(m_vOutEdge.begin()+i);
            edge->from=NULL; //disconnect from the edge
            return;
          }
    }

  //edge.to==this means edge is an incoming edge
  else if(edge->to==this)
    {
      for(unsigned int i=0; i<m_vInEdge.size(); ++i)
        if(m_vInEdge[i] == edge)
          {
            m_vInEdge.erase(m_vInEdge.begin()+i);
            edge->to=NULL; //disconnect from the edge
            return;
          }
    }
}

//------------------------------------------------------------

void Node::add(SubNode* subnode)
{
  //Check if subnode is valid
  if(!subnode)
    return;

  //And not already present
  for(unsigned int i=0; i<m_vSubNode.size(); ++i)
    if(m_vSubNode[i]==subnode)
        return;

  subnode->pOwner=this;
  m_vSubNode.push_back(subnode);
}

//------------------------------------------------------------

void Node::disconnect(SubNode* subnode)
{
  for(unsigned int i=0; i<m_vSubNode.size(); ++i)
    if(m_vSubNode[i]==subnode)
      {
        m_vSubNode.erase(m_vSubNode.begin()+i);
        subnode->pOwner=NULL; //disconnect the subnode
        return;
      }
}

//------------------------------------------------------------

void Node::remove(SubNode* subnode)
{
  for(unsigned int i=0; i<m_vSubNode.size(); ++i)
    if(m_vSubNode[i]==subnode)
      {
        m_vSubNode.erase(m_vSubNode.begin()+i);
        subnode->pOwner=NULL; //prevent the subnode to disconnect itself
        delete subnode;
        return;
      }
}

//------------------------------------------------------------

string Node::typeToString() const
{
  switch(TYPE_NODE(m_eType))
    {
      case ENTITY:
        return "Entity";
      break;

      case SUBNODE:
        return "SubNode";
      break;

      default:
        return "";
      break;
    }
}

//------------------------------------------------------------

Tree<Node*> Node::getComposition(vector<Entity*> vHierarchy)
{
  Tree<Node *> v;
  v.node=this;

  for(unsigned int i=0; i<m_vOutEdge.size(); ++i)
      if(m_vOutEdge[i]->isValid())
        if(TYPE_EDGE(m_vOutEdge[i]->type())==Edge::HAS)
          if(m_vOutEdge[i]->canBeAccessedBy(vHierarchy))
            v.child.push_back(m_vOutEdge[i]->to->getComposition(vHierarchy));

  for(unsigned int i=0; i<m_vSubNode.size(); ++i)
      if(TYPE_SUBNODE(m_vSubNode[i]->type())==SubNode::DECORATOR)
        if(((Decorator*)m_vSubNode[i])->canBeAccessedBy(vHierarchy))
          v.child.push_back(m_vSubNode[i]->getComposition(vHierarchy));

  return v;
}



///////////////////////////////////////////////////////////////
//                        Sub Node                           //
///////////////////////////////////////////////////////////////

SubNode::~SubNode()
{
  if(pOwner)
    pOwner->disconnect(this);
}

//------------------------------------------------------------

string SubNode::typeToString() const
{
  switch(TYPE_SUBNODE(m_eType))
    {
      case TERMINAL:
        return "Terminal";
      break;

      case DECORATOR:
        return "Decorator";
      break;

      default:
        return "";
      break;
    }
}

//------------------------------------------------------------

Entity*SubNode::getParent() const
{
  if(pParent)
    return pParent;

  Node* nFrom=pOwner;
  while(TYPE_NODE(nFrom->type())==Node::SUBNODE) //type!=SubNode means type==Entity
    nFrom=((SubNode*)nFrom)->pOwner;

  return (Entity*)nFrom;
}

//------------------------------------------------------------

SubNode*SubNode::fromString(string& stringFile)
{
  int i = stringFile.find(";");
  string sTemp = stringFile.substr(0,i);
  Decorator::Type type = (Decorator::Type) atoi(sTemp.c_str());

  if(TYPE_SUBNODE(type)==SubNode::DECORATOR)
    {
      Decorator* d=new Decorator(NULL,type);
      d->setFromString(stringFile);
      return d;
    }
  else if(TYPE_SUBNODE(type)==SubNode::TERMINAL)
    {
      Terminal* t=new Terminal("",NULL);
      t->setFromString(stringFile);
      return t;
    }

  return NULL;
}



///////////////////////////////////////////////////////////////
//                       Decorator                           //
///////////////////////////////////////////////////////////////


Decorator::Decorator(Node* owner, Type type, bool bPartition): SubNode("",owner), bPartition(bPartition)
{
  setType(type);
  sLabel=typeToString();
}

//------------------------------------------------------------

void Decorator::setType(int type)
{
  if(TYPE_SUBNODE(type)!=SubNode::DECORATOR)
    m_eType=TYPE_INVALID;
  else
    m_eType=type;
}

//------------------------------------------------------------

string Decorator::typeToString() const
{
  switch(TYPE_DECORATOR(m_eType))
    {
      case AND:
        return "And";
      break;

      case OR:
        return "Or";
      break;

      case XOR:
        return "Xor";
      break;

      default:
        return "";
      break;
    }
}

//------------------------------------------------------------

string Decorator::toString() const
{
  ostringstream s;

  s << m_eType << ";";
  s << ID << ";";
  s << bPartition << ";";
  s << AccessControl::toString();

  s << m_vSubNode.size() << ";";
  for(unsigned int i=0; i<m_vSubNode.size(); i++)
    s << m_vSubNode[i]->toString();

  return s.str();
}

//------------------------------------------------------------

void Decorator::setFromString(string& stringFile)
{
  string sTemp;
  read(sTemp,stringFile);
  m_eType=atoi(sTemp.c_str());

  read(sTemp,stringFile);
  ID=atoi(sTemp.c_str());

  read(sTemp,stringFile);
  bPartition=atoi(sTemp.c_str());

  AccessControl::setFromString(stringFile);

  read(sTemp,stringFile);
  int SubNode = atoi(sTemp.c_str());

  for(int i=0; i<SubNode; i++)
    add(SubNode::fromString(stringFile));
}

//------------------------------------------------------------

Decorator&Decorator::operator=(const Decorator& e)
{
  m_eType=e.m_eType;
  bPartition=e.bPartition;
  sLabel=e.sLabel;

  return *this;
}



///////////////////////////////////////////////////////////////
//                        Terminal                           //
///////////////////////////////////////////////////////////////

void Terminal::setType(int type)
{
  if(TYPE_SUBNODE(type)!=SubNode::TERMINAL)
    m_eType=TYPE_INVALID;
  else
    m_eType=type;
}

//------------------------------------------------------------

string Terminal::typeToString() const
{
  return SubNode::typeToString(); //no specific type for now
}

//------------------------------------------------------------

string Terminal::toString() const
{
  ostringstream s;

  s << m_eType << ";";
  s << ID << ";";
  s << sLabel << ";";
  s << AccessControl::toString();

  s << m_vSubNode.size() << ";";
  for(unsigned int i=0; i<m_vSubNode.size(); i++)
    s << m_vSubNode[i]->toString();

  return s.str();
}

//------------------------------------------------------------

void Terminal::setFromString(string& stringFile)
{
  string sTemp;
  read(sTemp,stringFile);
  m_eType = atoi(sTemp.c_str());

  read(sTemp,stringFile);
  ID= atoi(sTemp.c_str());

  read(sTemp,stringFile);
  sLabel = sTemp.c_str();

  AccessControl::setFromString(stringFile);

  read(sTemp,stringFile);
  int SubNode = atoi(sTemp.c_str());

  for(int i=0; i<SubNode; i++)
    add(SubNode::fromString(stringFile));
}



///////////////////////////////////////////////////////////////
//                         Entity                            //
///////////////////////////////////////////////////////////////

Entity::~Entity()
{
  if(pSemanticNet)
    pSemanticNet->remove(this);
}

//------------------------------------------------------------

void Entity::setType(int type)
{
  if(TYPE_NODE(type)!=Node::ENTITY)
    m_eType=TYPE_INVALID;
  else
    m_eType=type;
}

//------------------------------------------------------------

string Entity::typeToString() const
{
  switch (TYPE_ENTITY(m_eType))
    {
      case ATTRIBUTE:
        return "Attribute";
      break;

      case OBJECT:
        return "Object";
      break;

      case VALUE:
        return "Value";
      break;

      default:
        return "";
      break;
    }
}

//------------------------------------------------------------

string Entity::toString() const
{
  ostringstream s;

  s << m_eType << ";";
  s << ID << ";";
  s << sLabel << ";";

  s << (pConcrete ? 1:0) << ";";
  if(pConcrete)
    s << pConcrete->toString();

  s << (pNumerical ? 1:0) << ";";
  if(pNumerical)
    s << pNumerical->toString();

  s << m_vSubNode.size() << ";";

  for(unsigned int i=0; i<m_vSubNode.size(); i++)
    s << m_vSubNode[i]->toString();

  return s.str();
}

//------------------------------------------------------------

void Entity::setFromString(string& stringFile)
{
  string sTemp;

  read(sTemp,stringFile);
  m_eType = (Entity::Type) atoi(sTemp.c_str());

  read(sTemp,stringFile);
  ID= atoi(sTemp.c_str());

  read(sTemp,stringFile);
  sLabel = sTemp;

  read(sTemp,stringFile);
  bool concrete = atoi(sTemp.c_str());

  if(concrete)
    {
      setConcrete(true);
      pConcrete->setFromString(stringFile);
    }

  read(sTemp,stringFile);
  bool numerical = atoi(sTemp.c_str());

  if(numerical)
    {
      setNumerical(true);
      pNumerical->setFromString(stringFile);
    }

  read(sTemp,stringFile);
  int SubNode = atoi(sTemp.c_str());

  for(int i=0; i<SubNode; i++)
    add(SubNode::fromString(stringFile));
}

//------------------------------------------------------------

Entity&Entity::operator=(const Entity& e)
{
  sLabel=e.sLabel;
  m_eType=e.m_eType;

  m_vSubNode.insert(m_vSubNode.end(),e.getSubNode().begin(),e.getSubNode().end());

  setConcrete(e.pConcrete);
  if(pConcrete)
    pConcrete=e.pConcrete;

  setNumerical(e.pNumerical);
  if(pNumerical)
    pNumerical=e.pNumerical;

  return *this;
}

//------------------------------------------------------------

Tree<Entity*> Entity::getHierarchy()
{
  Tree<Entity*> t;
  t.node = this;

  for(unsigned int i=0; i<m_vOutEdge.size(); i++)
    if(m_vOutEdge[i]->isValid())
      if(TYPE_EDGE(m_vOutEdge[i]->type())==Edge::IS)
        if(TYPE_NODE(m_vOutEdge[i]->to->type())==Node::ENTITY)
        {
          Tree<Entity*> c=((Entity*)m_vOutEdge[i]->to)->getHierarchy();
          t.child.insert(t.child.begin(),c);
        }

  return t;
}

//------------------------------------------------------------

vector<Entity*> Entity::getVectorHierachy()
{
  Tree<Entity*> t=getHierarchy();
  vector<Entity*> v;

  treeToVector(t,v);

  return v;
}

//------------------------------------------------------------

Tree<Node*> Entity::getComposition()
{
  vector<Entity*> h=getVectorHierachy();

  Tree<Node *> v;
  v.node=this;

  for(unsigned int a=0; a<h.size();++a)
    {
      vector<Edge*> l=h[a]->getOutEdge();
      for(unsigned int i=0; i<l.size(); ++i)
        if( l[i]->Edge::isValid() && TYPE_EDGE(l[i]->type())==Edge::HAS)
          if(l[i]->canBeAccessedBy(h))
            v.child.push_back(l[i]->to->getComposition(h));

      vector<SubNode*> l2=h[a]->getSubNode();
      for(unsigned int i=0; i<l2.size(); ++i)
        if(TYPE_SUBNODE(l2[i]->type())==SubNode::DECORATOR)
          if(((Decorator*)l2[i])->canBeAccessedBy(h))
            v.child.push_back(l2[i]->getComposition(h));
    }

  return v;
}

//------------------------------------------------------------

void Entity::treeToVector(IN Tree<Entity*>& tree, OUT vector<Entity*>& vector) const
{

  vector.push_back(tree.node);

  for(unsigned int i=0; i<tree.child.size(); ++i)
    treeToVector(tree.child[i],vector);
}



///////////////////////////////////////////////////////////////
//                          Edge                             //
///////////////////////////////////////////////////////////////

Edge::Edge(Node* From, Type type, Node* To, int card): SemanticElement(EDGE_INVALID), from(From), to(To), iCard(card)
{
  setType(type);
  sLabel=EDGE_INVALID;
}

//------------------------------------------------------------

Edge::~Edge()
{
  if(from)
    from->disconnect(this);

  if(to)
    to->disconnect(this);

  if(pSemanticNet)
    pSemanticNet->remove(this);
}

//------------------------------------------------------------

Edge*Edge::newEdge(Node* From, Edge::Type type, Node* To)
{
  if(type == IS)
    return new EdgeIs(From,To);
  else if(type == HAS)
    return new EdgeHas(From,To);

  return NULL;
}

//------------------------------------------------------------

void Edge::setType(int type)
{
  if(TYPE_ELEMENT(type)!=SemanticElement::EDGE)
    m_eType=TYPE_INVALID;
  else
    m_eType=type;
}

//------------------------------------------------------------

string Edge::typeToString() const
{
  switch (TYPE_EDGE(m_eType))
    {
      case HAS:
        return "Possession";
      break;

      case IS:
        return "Definition";
      break;

      default:
        return "";
      break;
    }
}

//------------------------------------------------------------

Entity*Edge::getParent() const
{
  if(pParent)
    return pParent;

  Node* nFrom=from;
  while(TYPE_SUBNODE(nFrom->type())==Node::SUBNODE) //type!=Subnode means type==Entity
    nFrom=((SubNode*)nFrom)->pOwner;

  return (Entity*) nFrom;
}

//------------------------------------------------------------

string Edge::toString() const
{
  ostringstream s;

  s << m_eType << ";";
  s << from->ID << ";";
  s << to->ID << ";";
  s << iCard << ";";
  s << AccessControl::toString();

  s << (pConcrete ? 1:0) << ";";
  if(pConcrete)
    s << pConcrete->toString();

  return s.str();
}


//------------------------------------------------------------

void Edge::setFromString(string& stringFile)
{
  string sTemp;

  read(sTemp,stringFile);
  m_eType = (Edge::Type) atoi(sTemp.c_str());

  read(sTemp,stringFile);
  from=(Entity*)atoi(sTemp.c_str());

  read(sTemp,stringFile);
  to=(Entity*)atoi(sTemp.c_str());


  read(sTemp,stringFile);
  iCard=atoi(sTemp.c_str());
  //sLabel=createLabel();

  AccessControl::setFromString(stringFile);

  read(sTemp,stringFile);
  bool concrete = atoi(sTemp.c_str());

  if(concrete)
    {
      setConcrete(true);
      pConcrete->setFromString(stringFile);
    }
}

//------------------------------------------------------------

Edge&Edge::operator=(const Edge& e)
{
  m_eType=e.m_eType;
  iCard=e.iCard;

  setConcrete(e.pConcrete);
  if(pConcrete)
    pConcrete=e.pConcrete;

  sLabel=createLabel();

  return *this;
}

//------------------------------------------------------------

Edge*Edge::fromString(string& stringFile)
{
  int i = stringFile.find(";");
  string sTemp = stringFile.substr(0,i);
  Edge::Type type = (Edge::Type) atoi(sTemp.c_str());

  if(TYPE_EDGE(type)==Edge::IS)
    {
      EdgeIs* e=new EdgeIs(NULL,NULL);
      e->setFromString(stringFile);
      return e;
    }
  else if(TYPE_EDGE(type)==Edge::HAS)
    {
      EdgeHas* e=new EdgeHas(NULL,NULL);
      e->setFromString(stringFile);
      return e;
    }

  return NULL;
}



///////////////////////////////////////////////////////////////
//                        EdgeIs                             //
///////////////////////////////////////////////////////////////

string EdgeIs::createLabel() const
{
  if(!from||!to)
    return EDGE_INVALID;

  if(TYPE_SUBNODE(to->type())==SubNode::TERMINAL)
    return "Is";

  Node* nFrom=from;
  while(TYPE_NODE(nFrom->type())==Node::SUBNODE)
    nFrom=((SubNode*)from)->pOwner;

  Node* nTo=to;
  while(TYPE_NODE(nTo->type())==Node::SUBNODE)
    nTo=((SubNode*)to)->pOwner;

  Entity* eFrom=(Entity*) nFrom;
  Entity* eTo=(Entity*) nTo;

  if(TYPE_ENTITY(eFrom->type())!=TYPE_ENTITY(eTo->type()))
    return EDGE_INVALID;

  if(eFrom->pConcrete && !eTo->pConcrete)
      return "InstanceOf";
  else if(!eFrom->pConcrete && !eTo->pConcrete)
      return "IsA";

  return EDGE_INVALID;
}



///////////////////////////////////////////////////////////////
//                        EdgeHas                            //
///////////////////////////////////////////////////////////////

string EdgeHas::createLabel() const
{
  if(!from||!to)
    return EDGE_INVALID;

  Node* nFrom=from;
  while(TYPE_NODE(nFrom->type())==Node::SUBNODE)
    nFrom=((SubNode*)from)->pOwner;

  Node* nTo=to;
  while(TYPE_NODE(nTo->type())==Node::SUBNODE)
    nTo=((SubNode*)to)->pOwner;

  Entity* eFrom=(Entity*) nFrom;
  Entity* eTo=(Entity*) nTo;

  if(TYPE_ENTITY(eFrom->type())==Entity::OBJECT && TYPE_ENTITY(eTo->type())==Entity::ATTRIBUTE)
      return "Has Attribute";

  if(TYPE_ENTITY(eFrom->type())==Entity::ATTRIBUTE && TYPE_ENTITY(eTo->type())==Entity::ATTRIBUTE)
      return "Has Attribute";

  else if(TYPE_ENTITY(eFrom->type())==Entity::OBJECT && TYPE_ENTITY(eTo->type())==Entity::OBJECT)
    {
      ostringstream i;
      if(iCard != 1)
        i << " " << iCard;

      return "Has"+i.str();
    }

  else if(TYPE_ENTITY(eFrom->type())==Entity::ATTRIBUTE && TYPE_ENTITY(eTo->type())==Entity::VALUE)
      return "Has Value";

  return EDGE_INVALID;
}



///////////////////////////////////////////////////////////////
//                      Semantic net                         //
///////////////////////////////////////////////////////////////

SemanticNet::~SemanticNet()
{
  deleteAll();
}

//------------------------------------------------------------

void SemanticNet::remove(Edge *edge)
{
  for(unsigned int i=0; i<m_vEdge.size(); i++)
    if(m_vEdge[i] == edge)
      {
        m_vEdge.erase(m_vEdge.begin()+i);
        return;
      }
}

//------------------------------------------------------------

void SemanticNet::remove(Entity *entity)
{
  for(unsigned int i=0; i<m_vEntity.size(); i++)
    if(m_vEntity[i] == entity)
      {
        m_vEntity.erase(m_vEntity.begin()+i);
        return;
      }
}

//------------------------------------------------------------

void SemanticNet::deleteAll()
{
  // As edge can't exist without node, deleting all the entities
  // will delete all the node, subnodes and edges
  while( !m_vEntity.empty())
      //as deleting an element disconnect it from the semantic net,
      //no need to remove it from the vector
      delete m_vEntity.back();
}

//------------------------------------------------------------

void SemanticNet::add(Node *e, unsigned __int64 id)
{
  //don't add if already in the net
  if(e->pSemanticNet==this)
    return;

  e->pSemanticNet=this;

  if(id==ID_INVALID)
    e->ID = ++m_iLastID;
  else
    {
      e->ID = id;
      if( id > m_iLastID)
        m_iLastID = id;
    }

  if(TYPE_NODE(e->type())==Node::ENTITY)
    m_vEntity.push_back((Entity*)e);
}

//------------------------------------------------------------

void SemanticNet::add(Edge* e)
{
  //don't add if already in the net
  if(e->pSemanticNet==this)
    return;

  e->pSemanticNet=this;
  m_vEdge.push_back(e);
}

//------------------------------------------------------------

void SemanticNet::IDtoEntity(SubNode* n, std::map<unsigned __int64, Node*>& m)
{
  if(n->pParent)
    {
      if(Node* e=m[(__int64)n->pParent])
        {
          if(TYPE_NODE(e->type())==Node::ENTITY)
            n->pParent=(Entity*)e;
        }
      else
        n->pParent=NULL;
    }

  for(unsigned j=0; j<n->vBlackList.size();j++)
    if(Node* e=m[(__int64)n->vBlackList[j]])
      if(TYPE_NODE(e->type())==Node::ENTITY)
        n->vBlackList[j]=(Entity*)e;

  vector<SubNode*> s=n->getSubNode();
  for(unsigned int j=0; j<s.size(); j++)
    IDtoEntity(s[j],m);
}

//------------------------------------------------------------

void SemanticNet::IDtoEntity(Edge* e, std::map<unsigned __int64, Node*>& m)
{
  if(e->pParent)
    {
      if(Node* n=m[(__int64)e->pParent])
        {
          if(TYPE_NODE(n->type())==Node::ENTITY)
            e->pParent=(Entity*)n;
        }
      else
        e->pParent=NULL;
    }

  for(unsigned j=0; j<e->vBlackList.size();j++)
    if(Node* n=m[(__int64)e->vBlackList[j]])
      {
        if(TYPE_NODE(n->type())==Node::ENTITY)
          e->vBlackList[j]=(Entity*)n;
      }

  e->from=m[(__int64)e->from];
  e->to=m[(__int64)e->to];
  e->sLabel=e->createLabel();
}

//------------------------------------------------------------

void SemanticNet::saveNet(string path)
{
    ofstream f(path.c_str());

    assert(f);

    for(unsigned int i=0; i<m_vEntity.size(); i++)
        f << m_vEntity[i]->toString() << endl;

    f << endl;

    for(unsigned int i=0; i<m_vEdge.size(); i++)
        f << m_vEdge[i]->toString() << endl;

    f.close();
}

//------------------------------------------------------------

void SemanticNet::openNet(string path)
{
  deleteAll();

  fstream f(path.c_str());

  assert(f);

  std::map<unsigned __int64, Node*> m;
  string stringFile;
  getline(f,stringFile);

  while(stringFile.size()>2)
    {
      Entity* e=new Entity("",(Entity::Type)TYPE_INVALID);
      e->setFromString(stringFile);

      m[e->ID]=e;
      for(unsigned int i=0; i<e->getSubNode().size(); ++i)
        m[e->getSubNode()[i]->ID]=e->getSubNode()[i];

      add(e,e->ID);

      getline(f,stringFile);
    }

  getline(f,stringFile);
  while(stringFile.size()>2)
    {
      Edge* edge=Edge::fromString(stringFile);
      add(edge);
      getline(f,stringFile);
    }

  for(unsigned int i=0; i<m_vEntity.size(); i++)
    {
      vector<SubNode*> n=m_vEntity[i]->getSubNode();
      for(unsigned int j=0; j<n.size(); j++)
        IDtoEntity(n[j],m);
    }

  for(unsigned int i=0; i<m_vEdge.size(); i++)
    IDtoEntity(m_vEdge[i],m);

  f.close();
}

//------------------------------------------------------------

void SemanticNet::changeOwner(SubNode* subnode, Node* newOwner)
{
  subnode->pOwner->disconnect(subnode);
  newOwner->add(subnode);
}

