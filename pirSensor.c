/**
LooCI Copyright (C) 2013 KU Leuven.
All rights reserved.

LooCI is an open-source software development kit for developing and maintaining networked embedded applications;
it is distributed under a dual-use software license model:

1. Non-commercial use:
Non-Profits, Academic Institutions, and Private Individuals can redistribute and/or modify LooCI code under the terms of the GNU General Public License version 3, as published by the Free Software Foundation
(http://www.gnu.org/licenses/gpl.html).

2. Commercial use:
In order to apply LooCI in commercial code, a dedicated software license must be negotiated with KU Leuven Research & Development.

Contact information:
  Administrative Contact: Sam Michiels, sam.michiels@cs.kuleuven.be
  Technical Contact:           Danny Hughes, danny.hughes@cs.kuleuven.be
Address:
  iMinds-DistriNet, KU Leuven
  Celestijnenlaan 200A - PB 2402,
  B-3001 Leuven,
  BELGIUM. 
 */
/*
 * Copyright (c) 2010, Katholieke Universiteit Leuven
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *     * Neither the name of the Katholieke Universiteit Leuven nor the names of
 *       its contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "contiki.h"
#include "looci.h"
#include "pirSensor.h"
#include <stdint.h>
#include <avr/pgmspace.h>

#ifdef LOOCI_COMPONENT_DEBUG
#include "debug.h"
#else
#include "nodebug.h"
#endif

struct state {
	struct etimer et_poll;
	struct etimer et_send;
	uint8_t interval_poll;
	uint8_t interval_send;
	uint8_t next_value;
};
static const struct state initVar PROGMEM = {
	.interval_poll = 1,
	.interval_send = 5,
	.next_value = 0
};

#define LOOCI_COMPONENT_NAME pirSensor

COMPONENT_NO_INTERFACES();
//COMPONENT_INTERFACES(APPLICATION_EVENT_TYPE);
COMPONENT_NO_RECEPTACLES();
//COMPONENT_RECEPTACLES(APPLICATION_EVENT_TYPE);

#define LOOCI_NR_PROPERTIES 2
#define PROPERTY_ID_INTERVAL_POLL 1
#define PROPERTY_ID_INTERVAL_SEND 2

//format = {propertyId, datatype, offset, size, name}
LOOCI_PROPERTIES(
	{PROPERTY_ID_INTERVAL_POLL, DATATYPE_BYTE, offsetof(struct state, interval_poll), 1, NULL},
	{PROPERTY_ID_INTERVAL_SEND, DATATYPE_BYTE, offsetof(struct state, interval_send), 1, NULL}
);

LOOCI_COMPONENT_INIT("pirSensor", struct state, &initVar);

static uint8_t init(struct state* compState, void* data) {
	// Configure PB4 as input pin
	DDRB = (0 << DDB4);
	PORTB = (1 << PB4);

	return 1;
}

static uint8_t activate(struct state* compState, void* data) {
	PRINTF("PIR activate\r\n");

	// Start timers
	ETIMER_SET(&compState->et_poll, CLOCK_SECOND * compState->interval_poll );
	ETIMER_SET(&compState->et_send, CLOCK_SECOND * compState->interval_send );
	return 1;
}

static uint8_t deactivate(struct state* compState, void* data) {
	PRINTF("PIR deactivate\r\n");

	// Stop timers
	ETIMER_STOP(&compState->et_poll);
	ETIMER_STOP(&compState->et_send);
	return 1;
}

/*
 * Function called when etimer expires
 * Data contains the timer that expired
 */
static uint8_t time(struct state* compState, struct etimer* data) {
	uint8_t currentValue;

	if (data == &compState->et_poll) {
		// Read value
		currentValue = (PINB >> PB4) & 1;
		PRINTF("PIR value = %d\r\n", currentValue);
		// Store value for next send
		compState->next_value = compState->next_value || currentValue;
		// Reset timer
		ETIMER_SET(&compState->et_poll, CLOCK_SECOND * compState->interval_poll );
	} else if (data == &compState->et_send) {
		// Send value
		if (compState->next_value) {
			PRINTF("PIR send\r\n");
			// TODO Send
			// PUBLISH_EVENT(APPLICATION_EVENT_TYPE, &test, sizeof(test));
		}
		// Reset value
		compState->next_value = 0;
		// Reset timer
		ETIMER_SET(&compState->et_send, CLOCK_SECOND * compState->interval_send );
	}

	return 1;
}

static uint8_t event(struct state* compState, core_looci_event_t* event) {
	//uint8_t test = 1;
	//PUBLISH_EVENT(APPLICATION_EVENT_TYPE,&test,sizeof(test));
	return 1;
}

static uint8_t propertySet(struct state* compState,struct contiki_call* data) {
	return 1;
}


//FUNCTION DECLARATION
// You can comment out unused functions
COMP_FUNCS_INIT //THIS LINE MUST BE PRESENT
COMP_FUNC_INIT(init)
COMP_FUNC_ACTIVATE(activate)
COMP_FUNC_DEACTIVATE(deactivate)
COMP_FUNC_TIMER(time)
COMP_FUNC_EVENT(event)
COMP_FUNC_PROPERTY_IS_SET(propertySet)
COMP_FUNCS_END(NULL)//THIS LINE MUST BE PRESENT
