#pragma once

#include <time.h>
#include <stdio.h>

// Default location for FPS in a 640x480 resolution
// on the screen
#define DEFAULT_FPS_X 5
#define DEFAULT_FPS_Y 460

class FPS
{
public:
    FPS() :
      m_dFPS(0),
      m_dFrames(0),
      m_dMaxFPS(0),
      m_dSeconds(0),
      m_fAverageFPS(0)

    {
        m_clkStart = clock();
    }

    void update() {
        m_clkFinish = clock();
        if( (m_clkFinish - m_clkStart) >= CLOCKS_PER_SEC) {
            m_dFPS = m_dFrames;
            m_dFrames = 0;
            m_clkStart = clock();
            //sprintf_s(m_szFPSText, "FPS: %3.1f Avg.FPS: %3.2f Time: %3.0fs", m_dFPS, m_fAverageFPS, m_dSeconds);
            sprintf(m_szFPSText, "FPS: %3.1f Avg.FPS: %3.2f Time: %3.0fs", m_dFPS, m_fAverageFPS, m_dSeconds);

            if(m_dFPS > m_dMaxFPS)
            {
                m_dMaxFPS = m_dFPS;
            }

            if(m_dSeconds == 1)
            {
                m_fAverageFPS = (float)m_dFPS;
            }
            else if(m_dSeconds != 0)
            {
                m_fAverageFPS = (float)( (float)((m_dSeconds - 1) * m_fAverageFPS) / m_dSeconds) 
                                + (float)(m_dFPS / m_dSeconds);
            }

            m_dSeconds++;
        }
        else {
            m_dFrames++;
        }     
    }

    void FPSDiag() {
        printf("************************\n");
        printf("*  FPS Diagnostic Tool *\n");
        printf("*              ver 0.1 *\n");
        printf("************************\n");
        printf("  Maximum FPS: %3.1f\n", m_dMaxFPS);
        printf("  Average FPS: %3.2f\n", m_fAverageFPS);
        printf("  Seconds Run: %3.0f\n", m_dSeconds);
        printf("************************\n");
    }

private:
    clock_t m_clkStart;
    clock_t m_clkFinish;
    
    double m_dFPS;
    double m_dFrames;

    double m_dMaxFPS;

    // Average FPS
    double m_dSeconds;
    float m_fAverageFPS;

public:
    char m_szFPSText[40];
};