#include <raylib.h>
#include <rlImGui.h>
#include <extras/IconsFontAwesome6.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>

class Sample;

class Instance
{
public:
    Sample* sample;
    Sound sound;
    int startBeat;

    Instance(Sample* sample, Sound sound, int startBeat)
    {
        this->sample = sample;
        this->sound = sound;
        this->startBeat = startBeat;
    }
};

class Sample
{
public:
    std::string name;
    std::string path;
    Wave wave;

    Sample(std::string path)
    {
        this->path = path;
        this->name = GetFileName(path.c_str());
        this->wave = LoadWave(path.c_str());
    }

    Instance instance(int startBeat)
    {
        return Instance(this, LoadSoundFromWave(wave), startBeat);
    }
};

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "DAW by Vinny Horgan");
    SetTargetFPS(60);

    InitAudioDevice();

    rlImGuiSetup(true);

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    std::vector<Sample> samples;

    Sample sample1("assets/Melody Loops/Drill_Essentials_Apogee_140bpm_C#m.wav");
    samples.push_back(sample1);

    Sample sample2("assets/Melody Loops/Drill_Essentials_Balance_140bpm_Gm.wav");
    samples.push_back(sample2);

    Sound metronome = LoadSound("assets/metronome.mp3");

    float bpm = 140.0f;
    float beatLength = 60.0f / bpm;
    int trackLength = 16;

    bool playing = false;
    float currentTime = 0.0f;
    int currentBeat = 0;

    std::vector<Instance> instances;

    while (!WindowShouldClose())
    {
        if (playing)
        {
            currentTime += GetFrameTime();

            if (currentTime >= beatLength)
            {
                currentTime -= beatLength;
                currentBeat++;

                if (currentBeat >= trackLength)
                {
                    currentBeat = 0;

                    for (auto& instance : instances)
                    {
                        StopSound(instance.sound);
                    }
                }

                PlaySound(metronome);
            }

            for (auto& instance : instances)
            {
                if (instance.startBeat == currentBeat && !IsSoundPlaying(instance.sound))
                {
                    PlaySound(instance.sound);
                }
            }
        }

        if (IsFileDropped())
        {
            FilePathList droppedFiles = LoadDroppedFiles();

            for (unsigned int i = 0; i < droppedFiles.count; i++)
            {
                if (IsFileExtension(droppedFiles.paths[i], ".wav"))
                {
                    samples.push_back(Sample(droppedFiles.paths[i]));
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
            if (ImGui::Button(sample.name.c_str()))
            {
                instances.push_back(sample.instance(currentBeat));
            }
        }

        ImGui::End();

        ImGui::Begin("BPM");

        ImGui::SliderFloat("BPM", &bpm, 0.0f, 300.0f);

        beatLength = 60.0f / bpm;

        ImGui::End();

        ImGui::Begin("Track");

        ImGui::SliderInt("Track Length", &trackLength, 1, 64);

        if (ImGui::Button(ICON_FA_PLAY))
        {
            playing = true;

            for (auto& instance : instances)
            {
                PlaySound(instance.sound);
            }
        }

        ImGui::SameLine();

        if (ImGui::Button(ICON_FA_PAUSE))
        {
            playing = false;

            for (auto& instance : instances)
            {
                PauseSound(instance.sound);
            }
        }

        ImGui::SameLine();

        if (ImGui::Button(ICON_FA_STOP))
        {
            playing = false;
            currentTime = 0.0f;
            currentBeat = 0;

            for (auto& instance : instances)
            {
                StopSound(instance.sound);
            }
        }

        ImGui::SameLine();

        if (ImGui::Button(ICON_FA_FORWARD))
        {
            currentBeat++;
            currentTime = currentBeat * beatLength;
        }

        ImGui::SameLine();

        if (ImGui::Button(ICON_FA_BACKWARD))
        {
            currentBeat--;
            currentTime = currentBeat * beatLength;
        }

        ImGui::Text("Current Beat: %i", currentBeat);

        for (auto& instance : instances)
        {
            ImGui::Text("%s", instance.sample->name.c_str());

            ImGui::SameLine();

            ImGui::Text("%i", instance.startBeat);

            ImGui::SameLine();

            if (ImGui::Button(ICON_FA_TRASH))
            {
                StopSound(instance.sound);

                for (unsigned int i = 0; i < instances.size(); i++)
                {
                    if (instances[i].sample == instance.sample && instances[i].startBeat == instance.startBeat)
                    {
                        instances.erase(instances.begin() + i);
                        break;
                    }
                }
            }
        }

        ImGui::End();

        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();

    CloseAudioDevice();

    CloseWindow();

    return 0;
}

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
