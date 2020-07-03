window.addEventListener("DOMContentLoaded", function () {
  var fileIndex = 1;
  var highlightId = document.location.href.match(/#[^&#]+/);

  document.querySelectorAll(".linenos pre").forEach(function (block) {
    var thisFileIndex = fileIndex;
    ++fileIndex;
    var anchorExpr = "H-F" + thisFileIndex + "-L" + "$1";
    block.innerHTML = block.innerHTML.replace(
      /(\d+)/g,
      '<span id="' +
        anchorExpr +
        '"><a href="#' +
        anchorExpr +
        '">$1</a></span>'
    );
  });

  const classSet = {};
  document
    .querySelectorAll(".code .highlight pre .nc")
    .forEach(function (block) {
      const name = block.innerText;
      classSet[name] = classSet[name] || [];
      classSet[name].push(block);
    });

  for (const key in classSet) {
    const blocks = classSet[key];
    if (blocks.length == 1) {
      const anchorExpr = "H-NC-" + blocks[0].innerText;
      blocks[0].innerHTML =
        '<a href="#' + anchorExpr + '">' + blocks[0].innerHTML + "</a>";
      blocks[0].setAttribute("id", anchorExpr);
    }
  }

  document
    .querySelectorAll(".code .highlight pre .n")
    .forEach(function (block) {
      const previousElementSibling = block.previousElementSibling;
      if (!previousElementSibling) {
        return;
      }

      // already patched
      if (block.getAttribute("id")) {
        return;
      }

      if (previousElementSibling.innerText == "message") {
        const anchorExpr = "H-MSG-" + block.innerText;
        block.innerHTML =
          '<a href="#' + anchorExpr + '">' + block.innerHTML + "</a>";
        block.setAttribute("id", anchorExpr);
      } else if (previousElementSibling.innerText == "enum") {
        const anchorExpr = "H-ENUM-" + block.innerText;
        block.innerHTML =
          '<a href="#' + anchorExpr + '">' + block.innerHTML + "</a>";
        block.setAttribute("id", anchorExpr);
      }
    });

  setTimeout(function () {
    var highlightId = document.location.href.match(/#[^&#]+/);
    if (highlightId) {
      var ele = document.getElementById(highlightId[0].substr(1));
      if (ele && ele.parentNode && ele.parentNode.parentNode) {
        var parentTd = ele.parentNode.parentNode;
        if (parentTd.classList && parentTd.classList.contains("linenodiv")) {
          ele.classList.add("add-pointer");
        } else if (
          parentTd.className &&
          parentTd.className.match(/\blinenodiv\b/)
        ) {
          ele.className = (ele.className + " add-pointer").trim();
        }
      }
    }
  }, 250);
});
