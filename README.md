[![LinkedIn][linkedin-shield]][linkedin-url]

<!-- PROJECT LOGO -->
<div align="center">
  <a align="center">
    <img src="https://github.com/criogenox/B_ECC-Cpp-version_plot-capabilities_noGUI/assets/53323058/ad59ea63-6435-4b13-8efb-74729541e4ef.png" alt="Logo" width="500">
  </a>
  <h3 align="center">Railway applications &#x300A; A &#x300B;</h3>
  <p align="center">
    Wheel / Rail Contact Characterization Program
  </p>
</div>

## About the project

<div align="justify">
  <p>
The software capabilities focus on the characterization of wheel-rail rigid contact through the aid of following specific calculations:

- [ ] `Potential wheel-rail contact points detection` by mean of rigid profiles penetration comparison;
- [ ] `Contact angle tan(γa)` between wheel and rail (formerly, angle of the tangent to the point of contact);
- [ ] `Rolling-radius difference ∆r function`, at the predefined lateral amplitude interval;
- [ ] `Equivalent conicity tan(γc)=f(λ)` for each movement wavelength of a (equivalent) conical profile, at a specific lateral displacement (wave amplitude).
  
> For the EN15302 Standard scope & detalied computing processes aspects, please visit the [original JavaFx version][WRCC_java-url].
   </p>
   <div>

## Personal thoughts

<div align="justify">
  <p>
    
* `Why C++?:` std library is actually powerful, perhaps the most (I still like Java, as well).
* `Why this repo?:` to provide an example of a coded solution to a real engineering problem.
* `Ok, and what else?:` it's also an excellent hands-on opportunity to get to work on modern std library.

   </p>
    <p align="right">
    :muscle: don't let anyone get you down :muscle:
  </p> 
   <div>
     
## Built With

<div style="display: flex; flex-direction: column; align=center">
    <img class="img"src="https://github.com/criogenox/B_ECC-Cpp-version_plot-capabilities_noGUI/assets/53323058/1fdf2d22-fb04-45aa-9db0-8bd973942914.png" alt="Logo" width="90" height="80"/>
    <img class="img"src="https://github.com/criogenox/B_ECC-Cpp-version_plot-capabilities_noGUI/assets/53323058/6870b0b2-403c-49da-b745-5714b08f4a73.png" alt="Logo" width="90" height="80"/>
    <img class="img"src="https://github.com/criogenox/B_ECC-Cpp-version_plot-capabilities_noGUI/assets/53323058/7f7c66db-97e3-49a1-92d9-df41500b54ae.png" alt="Logo" width="90" height="90"/>

### Additional info
* Tested on Ubuntu 24.04 LTS & MS Windows 11.
* Standard template library features up to C++20.
* `Plot capabilities` enabled by default employing [matplotlib-cpp header-only library][matplotlib-cpp-url].
* Improved mathematical curves definition using cubic spline interpolation through the easy-to-use [cxx-spline header-only library for modern C++][cxx-spline-url].
* noGUI available (so far) || Qt or wxWidgets user interface planned.

###  Plotting output sample (with fully editable properties)

| ![Image 1](https://github.com/criogenox/A_WRCC-Cpp-version_plot-capabilities_noGUI/assets/53323058/5af1e805-2b26-4aaa-9e55-8bdfa09a0015.png) | ![Image 2](https://github.com/criogenox/A_WRCC-Cpp-version_plot-capabilities_noGUI/assets/53323058/a2c6d4c2-7c75-4649-a93c-4f38209eecbb.png) |
|:---:|:---:|
| ![Image 1](https://github.com/criogenox/A_WRCC-Cpp-version_plot-capabilities_noGUI/assets/53323058/956010b2-ffde-4987-a830-946a89c592a4.png) | ![Image 2](https://github.com/criogenox/A_WRCC-Cpp-version_plot-capabilities_noGUI/assets/53323058/28d5592c-16c7-4d8c-a106-8b08d94380ec.png) |

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[linkedin-shield]: https://user-images.githubusercontent.com/53323058/230575198-fa1acbf4-8f82-4d8e-b245-3979276bc240.png
[linkedin-url]: https://www.linkedin.com/in/criogenox/
[matplotlib-cpp-url]: https://github.com/lava/matplotlib-cpp/tree/master
[cxx-spline-url]: https://github.com/snsinfu/cxx-spline
[WRCC_java-url]: https://github.com/criogenox/A_WRCC-Wheel-Rail-Contact-Characterization
