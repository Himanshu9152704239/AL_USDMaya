//
// Copyright 2017 Animal Logic
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.//
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#pragma once

#include <pxr/pxr.h>
#include <string>
#include <memory>
#include <utility>
#include <vector>
#include <array>
#include "maya/MSceneMessage.h"

#include "AL/events/Events.h"


PXR_NAMESPACE_USING_DIRECTIVE
namespace AL {
namespace usdmaya {
namespace events {
class Listener;

typedef void* UserData;
typedef std::function<void(UserData*)> Callback;
typedef uintptr_t EventID;
typedef std::unique_ptr<Listener> ListenerPtr;
typedef std::vector<ListenerPtr> Listeners;
typedef std::array<Listeners, MayaEventType::kSceneMessageLast> ListenerContainer;

/*
 * \brief Objects of this class contain all the data needed to allow callbacks to happen
 */
struct Listener
{

  UserData userData = nullptr;  ///< data which is returned back to the user which registered this even and data
  Callback callback = nullptr;  ///< called when the event is triggerd

  MString command;              ///< Python or Mel command to call on callback
  MString tag;                  ///< tag or category of the event purpose

  struct
  {
    uint32_t weight : 31;       ///< order weight of this event
    uint32_t isPython : 1;      ///< if true (and the C++ fuction pointer is NULL), the command string will be treated as python, otherwise MEL
  };

  bool operator < (const Listener& event) const
    { return weight < event.weight; }
};


//----------------------------------------------------------------------------------------------------------------------
/// \brief Stores and orders the registered Event objects and executes these Events when the wanted Maya callbacks are triggered.
//----------------------------------------------------------------------------------------------------------------------
class MayaEventManager
{

public:
  explicit MayaEventManager();

  /// \brief Stores and orders the registered Maya callbacks. Internally creates a Listener type
  ///        and passes it to the corresponding register fnction
  /// \param event corresponding internal maya event
  /// \param callback function which will be called
  /// \param userData data which is returned to the user when the callback is triggered
  /// \param tag string to help classify the type of listener
  /// \param isPython true if the specified command should be executed as pythong
  /// \param command the string that will be executed when the callback happens
  /// \return the identifier of the created listener
  EventID registerCallback(MayaEventType event,
                           const Callback& callback,
                           uint32_t weight,
                           void* userData = nullptr,
                           const char* tag = "",
                           bool isPython = false,
                           const char* command = ""
                           );

  /// \brief Stores and orders the registered Maya callbacks
  /// \param eventType corresponding internal Maya event
  /// \param eventListener object which is executed when the corresponding Maya event triggers
  /// \return the identifier of the created listener
  EventID registerCallback(MayaEventType eventType, const Listener& eventListener);

  /// \brief Remove the corresponding EventID
  /// \param event internal type of the event
  /// \param id unique identifier of the Listener that was returned when it was registered
  /// \return true if an event was deregistered
  bool deregister(AL::usdmaya::events::MayaEventType event, EventID id);

  /// \brief Remove the corresponding EventID. More costly than the deregister
  ///        method where the MayaEventType is passed, since it has to search for the corresponding ID
  ///        in all the available events.
  /// \param id internal type of the event
  /// \return true if an event was deregistered
  bool deregister(EventID id);

  /// \brief retrieves the container containing all the Maya listeners
  /// \return ListenerContainer containing all the listeners
  ListenerContainer& listeners(){return m_mayaListeners;}

private:
  MCallbackId registerMayaCallback(MayaEventType eventType);
private:
  ListenerContainer m_mayaListeners;
  std::array<MCallbackId, MayaEventType::kSceneMessageLast> m_mayaCallbacks;

};

//----------------------------------------------------------------------------------------------------------------------
} // events
} // usdmaya
} // AL
//----------------------------------------------------------------------------------------------------------------------

