/** @type {import('tailwindcss').Config} */
export default {
    content: [
      "./index.html",
      "./src/**/*.{vue,js,ts,jsx,tsx}",
      "./node_modules/tw-elements-react/dist/js/**/*.js"
    ],
    theme: {
      extend: {},
    },
    plugins: [require("tw-elements-react/dist/plugin.cjs")],
  }