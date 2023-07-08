function checkWord() {
  var userInput = document.getElementById("wordInput").value;
  var resultElement = document.getElementById("result");

  if (userInput.toLowerCase() === "albert") {
    resultElement.textContent = "You entered the word 'Albert'.";
  } else {
    resultElement.textContent = "You did not enter the word 'Albert'.";
  }
}

var successSound = new Audio("success.wav");
var errorSound = new Audio("error.wav");
errorSound.volume = 0.2;

document.addEventListener("keydown", keydownListener);
function keydownListener(event) {
  var key = event.key;
  console.log(key);
  var inputField = document.getElementById("inputField");
  if (key === "Backspace" || key === "Delete") {
    console.log("delete");
    var divElements = inputField.querySelectorAll(".letter");
    console.log(divElements.length);
    if (divElements.length > 0) {
      var toRemove = divElements[divElements.length - 1];
      inputField.removeChild(toRemove);
    }
  } else if (key === "Enter") {
    //enter pressed
    var letters = inputField.querySelectorAll(".letter");
    var textString = "";
    for (var i = 0; i < letters.length; i++) {
      textString += letters[i].textContent.trim();
    }
    if (textString == "ALBERT") {
      document.querySelector("#inputField").innerHTML = "SUCCESS";
      document.querySelector("#instructions").remove();
      document.removeEventListener("keydown", keydownListener);
      successSound.play();
    } else {
      flash();
      errorSound.play();
    }
  } else if (/^[a-zA-Z]$/.test(key)) {
    //letter pressed
    var typedLetter = key.toUpperCase();
    var letterDiv = document.createElement("div");
    letterDiv.classList.add("letter");
    letterDiv.textContent = typedLetter;
    inputField.insertBefore(letterDiv, inputField.lastElementChild);
  } else {
    console.log("other");
    //other key pressed
    event.preventDefault();
  }
}

function flash() {
  var element = document.querySelector("#inputField");

  // Add the class
  element.classList.add("flash");

  // Create a promise that resolves after 5 seconds
  var delayPromise = new Promise(function (resolve) {
    setTimeout(resolve, 1100);
  });

  // Wait for the promise to resolve, then remove the class
  delayPromise.then(function () {
    element.classList.remove("flash");
  });
}
