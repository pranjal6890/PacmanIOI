#pragma once

#include <SFML/Audio.hpp>
#include <cmath>
#include <cstdint>
#include <functional>
#include <vector>


static const unsigned SAMPLE_RATE = 44100;

inline sf::SoundBuffer makeSoundBuffer(float duration,
                                       std::function<float(float, float)> gen, float vol = 1.0f)
{
  unsigned n = (unsigned)(SAMPLE_RATE * duration);
  std::vector<int16_t> s(n);
  for (unsigned i = 0; i < n; ++i)
  {
    float t = i / (float)SAMPLE_RATE;
    float v = std::max(-1.f, std::min(1.f, gen(t, duration) * vol));
    s[i] = (int16_t)(v * 32767.f);
  }
  sf::SoundBuffer buf;
  (void)buf.loadFromSamples(s.data(), n, 1, SAMPLE_RATE, {sf::SoundChannel::Mono});
  return buf;
}

inline sf::SoundBuffer makeBlopSound()
{
  return makeSoundBuffer(0.055f, [](float t, float d)
                         { return std::sin(2 * 3.14159f * (480.f + 320.f * (t / d)) * t) * std::exp(-t * 28.f); }, 0.35f);
}

inline sf::SoundBuffer makePowerupSound()
{
  return makeSoundBuffer(0.38f, [](float t, float d)
                         {
        float f=300.f+900.f*(t/d), e=(t<d*0.5f)?(t/(d*0.5f)):std::exp(-(t-d*0.5f)*8.f);
        return (std::sin(2*3.14159f*f*t)+0.4f*std::sin(4*3.14159f*f*t))*e; }, 0.55f);
}

inline sf::SoundBuffer makeShieldSound()
{
  return makeSoundBuffer(0.5f, [](float t, float)
                         {
        float e=std::exp(-t*5.f);
        return (std::sin(2*3.14159f*523.f*t)+std::sin(2*3.14159f*659.f*t)+
                std::sin(2*3.14159f*784.f*t))/3.f*e; }, 0.6f);
}

inline sf::SoundBuffer makeGhostEatSound()
{
  return makeSoundBuffer(0.28f, [](float t, float)
                         {
        float f=600.f*std::exp(-t*9.f)+80.f, e=std::exp(-t*12.f);
        return (std::sin(2*3.14159f*f*t)+std::sin(2*3.14159f*f*2.7f*t)*0.4f)*e; }, 0.7f);
}

inline sf::SoundBuffer makeDeathSound()
{
  return makeSoundBuffer(1.1f, [](float t, float d)
                         {
        float f=600.f*std::pow(0.15f,t/d), e=(t<0.1f)?(t/0.1f):std::exp(-(t-0.1f)*2.2f);
        return std::sin(2*3.14159f*f*(1.f+0.04f*std::sin(2*3.14159f*8.f*t))*t)*e; }, 0.8f);
}

inline sf::SoundBuffer makeWinSound()
{
  float notes[] = {523.25f, 659.25f, 783.99f, 1046.5f};
  unsigned total = (unsigned)(SAMPLE_RATE * 1.2f);
  std::vector<int16_t> s(total, 0);
  for (int n = 0; n < 4; ++n)
  {
    float start = n * 0.22f, f = notes[n];
    for (unsigned i = 0; i < total; ++i)
    {
      float t = i / (float)SAMPLE_RATE - start;
      if (t < 0 || t > 0.35f)
        continue;
      float e = (t < 0.01f) ? t / 0.01f : std::exp(-(t - 0.01f) * 4.5f);
      float v = (std::sin(2 * 3.14159f * f * t) + 0.5f * std::sin(4 * 3.14159f * f * t) +
                 0.25f * std::sin(6 * 3.14159f * f * t)) /
                1.75f * e * 0.7f;
      s[i] += (int16_t)(std::max(-1.f, std::min(1.f, v)) * 32767.f);
    }
  }
  sf::SoundBuffer buf;
  (void)buf.loadFromSamples(s.data(), total, 1, SAMPLE_RATE, {sf::SoundChannel::Mono});
  return buf;
}
