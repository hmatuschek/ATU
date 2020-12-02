#include "swr.h"


void swr_init() {
  ADMUX  |= ((0<<REFS1) | (0<<REFS1));
  ADCSRA |= ((1<<ADPS2) | (1<<ADPS1) | (0<<ADPS0));
}

void swr_read(uint8_t *swr, uint8_t *pwr) {
  int32_t fwd=0, rev=0;

  // enable ADC
  ADCSRA |= (1<<ADEN);
  for (uint8_t n=0; n<64; n++) {
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
  // disable ADC
  ADCSRA &= ~(1<<ADEN);

  *swr = 10;
  if (fwd > 1)
    *swr = (10*fwd+10*rev)/(fwd-rev);

  fwd *= 10;
  fwd = (fwd >> 16);
  fwd *= fwd;
  *pwr = fwd / 500;
}


uint8_t pwr_read() {
  int32_t fwd=0;

  // enable ADC
  ADCSRA |= (1<<ADEN);
  for (uint8_t n=0; n<64; n++) {
    // select FWD channel
    ADMUX = (ADC_FWD_REF | ADC_FWD_CHANNEL);
    // start conversion
    ADCSRA |= (1<<ADSC);
    while ((ADCSRA & (1<<ADSC))) {
      // wait...
    }
    fwd += ADC;
  }
  // disable ADC
  ADCSRA &= ~(1<<ADEN);

  fwd *= 10;
  fwd = (fwd>>16);
  fwd *= fwd;
  fwd /= 500;
  return fwd;
}
