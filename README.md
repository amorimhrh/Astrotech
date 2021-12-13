# Astrotech
![GitHub](https://img.shields.io/github/license/amorimhrh/Astrotech)
![Lines of code](https://img.shields.io/tokei/lines/github/amorimhrh/Astrotech)

<p align="center">
  <img src="https://github.com/amorimhrh/Astrotech/blob/dev/READMEImages/photo-a.gif?raw=true" alt="Astrotech Main Menu"/>
  <img src="https://github.com/amorimhrh/Astrotech/blob/dev/READMEImages/photo-b.gif?raw=true" alt="Astrotech Annihilator"/>
  <img src="https://github.com/amorimhrh/Astrotech/blob/dev/READMEImages/photo-c.gif?raw=true" alt="Astrotech Planet"/>
</p>

## Introduction

Astrotech is an educational Virtual Reality game created with the Unreal Engine for the scientific research project **"Development of Immersive Technologies Applied to Astrobiology Teaching"**. This experience focuses specifically on the teaching of Astrobiology.

On this GitHub repo, you can see the specific project files, scripts and blueprints that were used to create the experience. This repo is licensed under the MIT License, so you're free to use it under its terms.

## How To Install Unreal Project

Clone this repository onto your machine, and open up the **MachineCubes.uproject** using UE 4.27.

## Levels

The project opens up on the Main Menu level, located in the **Content/Levels** folder. On the same folder, two other levels can be seen, those being used as control environments for testing code on different terrains.

The **Content/Levels/CinematicLevels** directory contains levels with Level Sequences, and were used to record footage for presentations and/or videos.

The **Content/Levels/Planets** folder contains 4 examples of different planets, all generated procedurally using the Marching Cubes algorithm.

Finally, the **Content/Level/Spaceships** directory has various versions of the SLSS.

## Files

### 3D Models

3D models created by the author can be seen inside the **Content/Meshes** folder. 

### Blueprints

Blueprints designed by the author can be found both in the **Content/Blueprints** folder.

### C++ Scripts

C++ Scripts written by the author can be seen inside the **Source/MarchingCubes** folder.

### Effects

Effects made by the author can be found inside the **Content/FX** folder.

### Materials

Most materials created by the author can be seen inside the **Content/Materials** folder.

### Textures

Textures created by the author can be found inside the **Content/Textures** folder.

## External Plugins Used

 * [Runtime Mesh Component](https://github.com/TriAxis-Games/RuntimeMeshComponent)

## Research

If you wish to know more about the research itself, you can read the full Research Paper [here](https://bit.ly/ditaat-lite), or read the abstract below:

### Abstract

Immersive technologies have been evolving and influencing the means of human interaction. In this context, the development of Immersive Environments can contribute to education, aiding in universalizing access and developing Scientific Literacy, alongside skills and competencies of BNCC and ENEM. In addition, practices of this nature can promote digital literacy, critical thinking and contribute to the development of autonomy. That said, the objective of this project is to develop a High Immersion Environment and verify the potentialities and challenges of its use in Astrobiology teaching. For this, we divided the methodology into three phases. In the first one, the Pedagogical Approach, we surveyed Astrobiology concepts addressed in Basic Education that could serve as a basis for the virtual environment's development. As a result, we found 28 concepts and 34 skills and capabilities related to the theme. The second phase was responsible for the Environment's Planning. At that time, we developed both a particular document for the Immersive Environments' design, the IDD, based on Game Design Documents (GDD), and a specific version of the IDD for the project. Based on that document, we started the Environment Development phase on Unreal Engine. As a result, we defined the content to be addressed in the experience and made the narrative's universe: the main menu screen, a spaceship, and planets with different procedurally generated terrains made with the Marching Cubes algorithm and functions that use Perlin Noise, Fractional Brownian Motion, and Domain Warping, all implemented via C++. In addition, we developed interaction and locomotion in Virtual Reality through Unreal's Blueprints. Subsequently, we created meshes of an alien and a DNA molecule, made using Medium, and a wrench and interactive table designed through Blender. Furthermore, we also made human characters using the MetaHuman Creator platform. So far, the results show that immersive development is feasible and can bring notable contributions to Science Education.

Keywords: Technology, Education, Virtual Reality, Investigative Teaching, Astrobiology