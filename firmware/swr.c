#include "swr.h"


void swr_init() {
  ADMUX  |= ((0<<REFS1) | (0<<REFS1));
  ADCSRA |= ((1<<ADPS2) | (1<<ADPS1) | (0<<ADPS0));
}

void swr_start() {
  // enable ADC
  ADCSRA |= (1<<ADEN);
}

void swr_end() {
  // disable ADC
  ADCSRA &= ~(1<<ADEN);
}

uint16_t swr_read() {
  int32_t fwd=0, rev=0, pwr=0;
  for (uint8_t i=0; i<SWR_NUM_MEASURE; i++) {
    // select FWD channel
    ADMUX = (ADC_FWD_REF | ADC_FWD_CHANNEL);
    // start conversion
    ADCSRA |= (1<<ADSC);
    while ((ADCSRA & (1<<ADSC))) {
      // wait...
    }
    fwd += ADC;

    // select REV channel
    ADMUX = (ADC_REV_REF | ADC_REV_CHANNEL);
    // start conversion
    ADCSRA |= (1<<ADSC);
    while ((ADCSRA & (1<<ADSC))) {
      // wait...
    }
    rev += ADC;
  }

  fwd /= SWR_NUM_MEASURE;
  rev /= SWR_NUM_MEASURE;

  // Compute power
  pwr = fwd*500;
  pwr = (pwr>>10);
  pwr *= pwr;
  pwr /= 50;

  uint16_t swr = 9999;
  if ((pwr >= MIN_POWER) && (rev<fwd))
    swr = (100*fwd+100*rev)/(fwd-rev);
  else if (pwr <= MIN_POWER)
    return 0;
  return swr;
}


uint16_t pwr_read() {
  int32_t fwd=0;

  for (uint8_t n=0; n<SWR_NUM_MEASURE; n++) {
    // select FWD channel
    ADMUX = (ADC_FWD_REF | ADC_FWD_CHANNEL);
    // start conversion
    ADCSRA |= (1<<ADSC);
    while ((ADCSRA & (1<<ADSC))) {
      // wait...
    }
    fwd += ADC;
  }
  fwd /= SWR_NUM_MEASURE;

  fwd *= 500;
  fwd = (fwd>>10);
  fwd *= fwd;
  fwd /= 50;
  return fwd;
}
