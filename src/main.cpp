#include <raylib.h>
#include <rlImGui.h>
#include <extras/IconsFontAwesome6.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>

float bpm = 140.0f;
float beatLength = 60.0f / bpm;

Wave mixWaveFiles(Wave wave1, Wave wave2)
{
    Wave mixedWave = {};
    mixedWave.sampleRate = wave1.sampleRate;
    mixedWave.sampleSize = wave1.sampleSize;
    mixedWave.channels = wave1.channels;

    if (wave1.frameCount > wave2.frameCount)
    {
        mixedWave.frameCount = wave1.frameCount;
    }
    else
    {
        mixedWave.frameCount = wave2.frameCount;
    }

    mixedWave.data = (void*)malloc(mixedWave.frameCount * mixedWave.channels * mixedWave.sampleSize / 8);

    for (unsigned int i = 0; i < mixedWave.frameCount; i++)
    {
        for (unsigned int j = 0; j < mixedWave.channels; j++)
        {
            switch (mixedWave.sampleSize)
            {
                case 32:
                {
                    float sample1 = 0.0f;
                    float sample2 = 0.0f;

                    if (i < wave1.frameCount)
                    {
                        sample1 = ((float*)wave1.data)[i * wave1.channels + j];
                    }

                    if (i < wave2.frameCount)
                    {
                        sample2 = ((float*)wave2.data)[i * wave2.channels + j];
                    }

                    ((float*)mixedWave.data)[i * mixedWave.channels + j] = sample1 + sample2;

                    break;
                }
                case 16:
                {
                    short sample1 = 0;
                    short sample2 = 0;

                    if (i < wave1.frameCount)
                    {
                        sample1 = ((short*)wave1.data)[i * wave1.channels + j];
                    }

                    if (i < wave2.frameCount)
                    {
                        sample2 = ((short*)wave2.data)[i * wave2.channels + j];
                    }

                    ((short*)mixedWave.data)[i * mixedWave.channels + j] = sample1 + sample2;

                    break;
                }
                case 8:
                {
                    unsigned char sample1 = 0;
                    unsigned char sample2 = 0;

                    if (i < wave1.frameCount)
                    {
                        sample1 = ((unsigned char*)wave1.data)[i * wave1.channels + j];
                    }

                    if (i < wave2.frameCount)
                    {
                        sample2 = ((unsigned char*)wave2.data)[i * wave2.channels + j];
                    }

                    ((unsigned char*)mixedWave.data)[i * mixedWave.channels + j] = sample1 + sample2;

                    break;
                }
            }
        }
    }

    return mixedWave;
}

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "WAD by Vinny Horgan");
    SetTargetFPS(60);

    InitAudioDevice();

    rlImGuiSetup(true);

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    std::map<std::string, Wave> samples;

    Wave wave = LoadWave("assets/Melody Loops/Drill_Essentials_Apogee_140bpm_C#m.wav");
    samples["assets/Melody Loops/Drill_Essentials_Apogee_140bpm_C#m.wav"] = wave;

    Wave wave2 = LoadWave("assets/Melody Loops/Drill_Essentials_Balance_140bpm_Gm.wav");
    samples["assets/Melody Loops/Drill_Essentials_Balance_140bpm_Gm.wav"] = wave2;

    Wave mixedWave = mixWaveFiles(wave, wave2);
    samples["mixed"] = mixedWave;

    Sound metronome = LoadSound("assets/metronome.mp3");

    float time = 0.0f;

    while (!WindowShouldClose())
    {
        time += GetFrameTime();

        if (time >= beatLength)
        {
            PlaySound(metronome);
            time = 0.0f;
        }

        if (IsFileDropped())
        {
            FilePathList droppedFiles = LoadDroppedFiles();

            for (unsigned int i = 0; i < droppedFiles.count; i++)
            {
                if (IsFileExtension(droppedFiles.paths[i], ".wav"))
                {
                    Wave wave = LoadWave(droppedFiles.paths[i]);
                    samples[droppedFiles.paths[i]] = wave;
                }
            }

            UnloadDroppedFiles(droppedFiles);
        }

        BeginDrawing();

        ClearBackground(BLACK);

        rlImGuiBegin();

        ImGui::DockSpaceOverViewport();

        ImGui::Begin("Loaded samples");

        for (auto& sample : samples)
        {
            if (ImGui::Button(sample.first.c_str()))
            {
                Sound sound = LoadSoundFromWave(sample.second);
                PlaySound(sound);
            }
        }

        ImGui::End();

        ImGui::Begin("BPM");

        ImGui::SliderFloat("BPM", &bpm, 0.0f, 300.0f);

        beatLength = 60.0f / bpm;

        ImGui::End();

        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();

    CloseAudioDevice();

    CloseWindow();

    return 0;
}
