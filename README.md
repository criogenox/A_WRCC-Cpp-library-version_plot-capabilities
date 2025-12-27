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

## About the project (Qt6 GUI update in progress)

<div align="justify">
  <p>
The software capabilities focus on the characterization of wheel-rail rigid contact through the aid of following specific calculations:

- [x] `Potential wheel-rail contact points detection.`
- [x] `Contact angle tan(γa).`
- [x] `Rolling-radius difference ∆r function.`
- [x] `Equivalent conicity tan(γc)=f(λ).`

###  Plotting output sample (with fully editable properties)

| ![Image 1](https://github.com/criogenox/A_WRCC-Cpp-version_plot-capabilities_noGUI/assets/53323058/6eb3c726-e97c-4d3c-a0c0-5c15279ea657.png) | ![Image 2](https://github.com/criogenox/A_WRCC-Cpp-version_plot-capabilities_noGUI/assets/53323058/bc4ec8c1-ed3a-4e2b-8868-91ba4e5595e9.png) |
|:---:|:---:|
| ![Image 1](https://github.com/criogenox/A_WRCC-Cpp-version_plot-capabilities_noGUI/assets/53323058/d1234b59-c556-49bb-b34f-91274ac097ee.png) | ![Image 2](https://github.com/criogenox/A_WRCC-Cpp-version_plot-capabilities_noGUI/assets/53323058/9963db95-3261-44e3-982f-9e1d6f609c7f.png) |

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
* Heavy use of smart pointers for the sake of design simplicity.
* `Plot capabilities` enabled by default employing [matplotlib-cpp header-only library][matplotlib-cpp-url].
* Improved mathematical curves definition using monotone cubic Hermite interpolator with PCHIP (Fritsch–Carlson) slope construction.

<!-- LICENSE -->
## License

- User interface distributed under the GPL-3.0 License. See [GUI-LICENSE.txt][gui-license-url] for more information.
- Calculation libraries distributed under the MIT License. See [LIB-LICENSE.txt][lib-license-url] for more information.

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[linkedin-shield]: https://user-images.githubusercontent.com/53323058/230575198-fa1acbf4-8f82-4d8e-b245-3979276bc240.png
[linkedin-url]: https://www.linkedin.com/in/criogenox/
[matplotlib-cpp-url]: https://github.com/lava/matplotlib-cpp/tree/master
[WRCC_java-url]: https://github.com/criogenox/A_WRCC-Wheel-Rail-Contact-Characterization
[gui-license-url]: https://github.com/criogenox/A_WRCC-Cpp-library-version_plot-capabilities?tab=GPL-3.0-1-ov-file
[lib-license-url]: https://github.com/criogenox/A_WRCC-Cpp-library-version_plot-capabilities?tab=MIT-2-ov-file
