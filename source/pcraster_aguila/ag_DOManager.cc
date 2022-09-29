#include "ag_DOManager.h"
#include <algorithm>
#include "ag_DataObject.h"



/*!
  \file
  brief

  more elaborated
*/



//------------------------------------------------------------------------------
// DEFINITION OF STATIC CLASS MEMBERS
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// DEFINITION OF CLASS MEMBERS
//------------------------------------------------------------------------------

ag::DOManager::DOManager()
{
}



ag::DOManager::~DOManager()
{
  for(auto & d_dataObject : d_dataObjects) {
#ifdef DEBUG_DEVELOP
    assert(d_dataObject->nrObservers() == 0);
#endif
    delete d_dataObject;
  }
}



/*!
  \return    The newly created data object.
  \warning   Never delete the returned pointer! Deletion of data objects will
             be taken care of.
  \sa        deleteDataObject(), copyDataObject()

  The created data object is added to the collection, as a side effect.
*/
ag::DataObject *ag::DOManager::newDataObject()
{
  auto *o = new ag::DataObject();
  d_dataObjects.push_back(o);
  return o;
}



/*!
  \param     o Data object to delete.
  \sa        newDataObject(), copyDataObject()
*/
void ag::DOManager::deleteDataObject(ag::DataObject *o)
{
  assert(o);
#ifdef DEBUG_DEVELOP
  assert(o->nrObservers() == 0);
#endif

  auto it = std::find(d_dataObjects.begin(), d_dataObjects.end(), o);
  assert(it != d_dataObjects.end());
  delete *it;
  d_dataObjects.erase(it);
}



/*!
  \param     o Data object to copy.
  \return    Newly created data object with the same data as \a o.
  \warning   Never delete the returned pointer! Deletion of data objects will
             be taken care of.
  \sa        newDataObject(), deleteDataObject()

  The created data object is added to the collection, as a side effect.
*/
/*
ag::DataObject *ag::DOManager::copyDataObject(
                   const ag::DataObject *rhs)
{
  ag::DataObject *o = new ag::DataObject(*rhs);
  d_dataObjects.push_back(o);
  return o;
}
*/



ag::DOManager::const_iterator ag::DOManager::begin() const
{
  return d_dataObjects.begin();
}



ag::DOManager::iterator ag::DOManager::begin()
{
  return d_dataObjects.begin();
}



ag::DOManager::const_iterator ag::DOManager::end() const
{
  return d_dataObjects.end();
}



ag::DOManager::iterator ag::DOManager::end()
{
  return d_dataObjects.end();
}



size_t ag::DOManager::nrDataObjects()
{
  return d_dataObjects.size();
}



#ifdef DEBUG_DEVELOP
void ag::DOManager::checkIntegrity()
{
  // Let's check if all data objects have observers. A data object without an
  // observer is a waste of space.
  for(auto & d_dataObject : d_dataObjects)
  {
    assert(d_dataObject->nrObservers() > 0);
  }
}
#endif



//------------------------------------------------------------------------------
// DEFINITION OF FREE OPERATORS
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// DEFINITION OF FREE FUNCTIONS
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// DOCUMENTATION OF ENUMERATIONS
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// DOCUMENTATION OF INLINE FUNCTIONS
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// DOCUMENTATION OF PURE VIRTUAL FUNCTIONS
//------------------------------------------------------------------------------


